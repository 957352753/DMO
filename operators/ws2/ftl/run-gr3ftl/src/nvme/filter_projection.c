#include "filter_projection.h"

#include <stddef.h>
#include <string.h>

#include "filter_tuple.h"
#include "page.h"

#define FILTER_PROJECTION_ALIGNMENT 4U

typedef struct filter_projection_runtime
{
	filter_projection_desc_t config;
	unsigned char *result;
	uint32_t result_bytes;
	uint32_t write_offset;
	uint32_t wanted_mask;
	filter_projection_result_header_t *header;
} filter_projection_runtime_t;

static filter_projection_runtime_t projection_runtime;

typedef char projection_desc_size_must_be_64[
	(sizeof(filter_projection_desc_t) == 64U) ? 1 : -1];
typedef char projection_result_header_size_must_be_128[
	(sizeof(filter_projection_result_header_t) == 128U) ? 1 : -1];
typedef char projection_row_header_size_must_be_8[
	(sizeof(filter_projection_row_header_t) == 8U) ? 1 : -1];
typedef char projection_column_size_must_be_8[
	(sizeof(filter_projection_column_t) == 8U) ? 1 : -1];
typedef char projection_column_limit_must_match[
	(MAX_COL == FILTER_PROJECTION_MAX_COLUMNS) ? 1 : -1];

static uint32_t projection_align4(uint32_t value)
{
	return (value + (FILTER_PROJECTION_ALIGNMENT - 1U)) &
		   ~(FILTER_PROJECTION_ALIGNMENT - 1U);
}

static void projection_set_error(uint32_t error, uint32_t flags)
{
	if(!projection_runtime.header)
		return;

	projection_runtime.header->flags |= flags;
	if(projection_runtime.header->error_code == FILTER_PROJECTION_ERROR_NONE)
		projection_runtime.header->error_code = error;
}

void filter_projection_disable(void)
{
	memset(&projection_runtime, 0, sizeof(projection_runtime));
}

int filter_projection_configure(const filter_projection_desc_t *desc,
								uint32_t natts,
								uint32_t count_only)
{
	filter_projection_desc_t config;
	uint32_t seen = 0;

	filter_tuple_reset_decoder_cache();
	filter_projection_disable();
	if(!desc || desc->magic != FILTER_PROJECTION_DESC_MAGIC)
		return 0;

	memcpy(&config, desc, sizeof(config));
	if(config.max_row_bytes == 0U)
		config.max_row_bytes = FILTER_PROJECTION_DEFAULT_MAX_ROW_BYTES;
	if(config.version != FILTER_PROJECTION_DESC_VERSION ||
	   config.struct_bytes < sizeof(filter_projection_desc_t) ||
	   (config.mode != FILTER_PROJECTION_MODE_ARM &&
		config.mode != FILTER_PROJECTION_MODE_FPGA) ||
	   config.output_format != FILTER_PROJECTION_FORMAT_COMPACT_V1 ||
	   config.column_count == 0 || config.column_count > MAX_COL ||
	   natts == 0 || natts > MAX_COL || count_only ||
	   config.max_row_bytes < sizeof(filter_projection_row_header_t) +
			config.column_count * sizeof(filter_projection_column_t) ||
	   config.max_row_bytes > FILTER_PROJECTION_STAGING_BYTES_PER_PAGE)
	{
		return -1;
	}

	for(uint32_t i = 0; i < config.column_count; i++)
	{
		uint32_t attr = config.attr_index[i];

		if(attr >= natts || (seen & (1U << attr)) != 0U)
			return -1;
		seen |= 1U << attr;
	}

	projection_runtime.config = config;
	projection_runtime.wanted_mask = seen;
	return 1;
}

void filter_projection_reset_result(void *result_buffer, uint32_t result_bytes)
{
	filter_projection_result_header_t *header;
	uint32_t page_count = 1;

	if(!filter_projection_enabled() || !result_buffer ||
	   result_bytes < FILTER_PROJECTION_RESULT_DATA_OFFSET)
		return;

	memset(result_buffer, 0, result_bytes);
	projection_runtime.result = (unsigned char *)result_buffer;
	projection_runtime.result_bytes = result_bytes;
	projection_runtime.write_offset = FILTER_PROJECTION_RESULT_DATA_OFFSET;
	header = (filter_projection_result_header_t *)
		(projection_runtime.result + FILTER_PROJECTION_RESULT_HEADER_OFFSET);
	projection_runtime.header = header;

	memcpy(projection_runtime.result, &page_count, sizeof(page_count));
	header->magic = FILTER_PROJECTION_RESULT_MAGIC;
	header->version = FILTER_PROJECTION_RESULT_VERSION;
	header->header_bytes = sizeof(*header);
	header->requested_mode = projection_runtime.config.mode;
	header->effective_mode = FILTER_PROJECTION_EFFECTIVE_NONE;
	header->output_format = projection_runtime.config.output_format;
	header->column_count = projection_runtime.config.column_count;
	header->bytes_used = FILTER_PROJECTION_RESULT_DATA_OFFSET;
	header->max_row_bytes = projection_runtime.config.max_row_bytes;
	memcpy(header->attr_index, projection_runtime.config.attr_index, MAX_COL);
}

uint32_t filter_projection_enabled(void)
{
	return projection_runtime.config.mode != FILTER_PROJECTION_MODE_DISABLED;
}

uint32_t filter_projection_mode(void)
{
	return projection_runtime.config.mode;
}

uint32_t filter_projection_column_count(void)
{
	return projection_runtime.config.column_count;
}

uint32_t filter_projection_max_row_bytes(void)
{
	return projection_runtime.config.max_row_bytes;
}

const uint8_t *filter_projection_attr_index(void)
{
	return projection_runtime.config.attr_index;
}

static int projection_locate_values(const unsigned char *tuple,
									uint32_t tuple_len,
									const tupdesc_t *tupdesc,
									uint32_t natts,
									filter_tuple_value_t *values)
{
	int rc;

	rc = filter_tuple_decode(tuple, tuple_len, tupdesc, natts,
		projection_runtime.wanted_mask, values);
	if(rc == -FILTER_TUPLE_ERROR_UNSUPPORTED_VARLENA)
		return -FILTER_PROJECTION_ERROR_UNSUPPORTED_VARLENA;
	if(rc < 0)
		return -FILTER_PROJECTION_ERROR_BAD_TUPLE;
	return 0;
}

static int projection_reserve_row(uint32_t row_bytes, unsigned char **row_out)
{
	uint32_t page_count;

	if(!projection_runtime.header || !row_out ||
	   row_bytes > projection_runtime.config.max_row_bytes ||
	   projection_runtime.write_offset > projection_runtime.result_bytes ||
	   row_bytes > projection_runtime.result_bytes - projection_runtime.write_offset)
	{
		projection_set_error(FILTER_PROJECTION_ERROR_RESULT_FULL,
						 FILTER_PROJECTION_RESULT_FLAG_FULL);
		return -1;
	}

	*row_out = projection_runtime.result + projection_runtime.write_offset;
	projection_runtime.write_offset += row_bytes;
	projection_runtime.header->bytes_used = projection_runtime.write_offset;
	page_count = (projection_runtime.write_offset + PG_PAGE_SIZE - 1U) / PG_PAGE_SIZE;
	if(page_count == 0U)
		page_count = 1U;
	memcpy(projection_runtime.result, &page_count, sizeof(page_count));
	return 0;
}

static void projection_record_mode(uint32_t mode, uint32_t rows)
{
	filter_projection_result_header_t *header = projection_runtime.header;

	if(!header || rows == 0U)
		return;

	if(mode == FILTER_PROJECTION_EFFECTIVE_ARM)
		header->arm_rows += rows;
	else if(mode == FILTER_PROJECTION_EFFECTIVE_FPGA)
		header->fpga_rows += rows;

	if(header->arm_rows != 0U && header->fpga_rows != 0U)
		header->effective_mode = FILTER_PROJECTION_EFFECTIVE_MIXED;
	else
		header->effective_mode = (uint8_t)mode;
	if(projection_runtime.config.mode == FILTER_PROJECTION_MODE_FPGA &&
	   mode == FILTER_PROJECTION_EFFECTIVE_ARM)
		header->flags |= FILTER_PROJECTION_RESULT_FLAG_FPGA_FALLBACK;
}

int filter_projection_append_arm(const unsigned char *page_data,
								 const unsigned short *tuple_offs,
								 const unsigned short *tuple_lens,
								 int hit_count,
								 const tupdesc_t *tupdesc,
								 uint32_t natts,
								 uint16_t pd_lower,
								 uint32_t *rows_written_out)
{
	uint32_t rows_written = 0;

	if(rows_written_out)
		*rows_written_out = 0U;
	if(!filter_projection_enabled() || !projection_runtime.header ||
	   !page_data || !tuple_offs || !tuple_lens || !tupdesc || natts > MAX_COL)
		return -1;
	if(hit_count < 0)
		hit_count = 0;
	if(hit_count > MAX_TUPLE_PER_PAGE)
		hit_count = MAX_TUPLE_PER_PAGE;

	for(int row_index = 0; row_index < hit_count; row_index++)
	{
		filter_tuple_value_t values[MAX_COL];
		filter_projection_row_header_t *row_header;
		filter_projection_column_t *columns;
		unsigned char *row;
		uint32_t row_bytes;
		uint32_t data_offset;
		uint32_t tuple_off = tuple_offs[row_index];
		uint32_t tuple_len = tuple_lens[row_index];
		int rc;

		if(tuple_off < pd_lower || tuple_off >= PG_PAGE_SIZE || tuple_len == 0U ||
		   tuple_len > PG_PAGE_SIZE - tuple_off)
			rc = -FILTER_PROJECTION_ERROR_BAD_TUPLE;
		else
			rc = projection_locate_values(page_data + tuple_off, tuple_len,
									  tupdesc, natts, values);
		if(rc < 0)
		{
			uint32_t error = (uint32_t)(-rc);

			projection_set_error(error, FILTER_PROJECTION_RESULT_FLAG_ARM_ERROR);
			projection_runtime.header->dropped_rows += (uint32_t)(hit_count - row_index);
			projection_record_mode(FILTER_PROJECTION_EFFECTIVE_ARM, rows_written);
			if(rows_written_out)
				*rows_written_out = rows_written;
			return -1;
		}

		row_bytes = sizeof(*row_header) +
			projection_runtime.config.column_count * sizeof(*columns);
		for(uint32_t col = 0; col < projection_runtime.config.column_count; col++)
		{
			uint32_t attr = projection_runtime.config.attr_index[col];
			if((values[attr].flags & FILTER_PROJECTION_COLUMN_FLAG_NULL) == 0U)
				row_bytes += projection_align4(values[attr].length);
		}
		row_bytes = projection_align4(row_bytes);

		if(projection_reserve_row(row_bytes, &row) < 0)
		{
			projection_runtime.header->dropped_rows += (uint32_t)(hit_count - row_index);
			projection_record_mode(FILTER_PROJECTION_EFFECTIVE_ARM, rows_written);
			if(rows_written_out)
				*rows_written_out = rows_written;
			return -1;
		}

		memset(row, 0, row_bytes);
		row_header = (filter_projection_row_header_t *)row;
		columns = (filter_projection_column_t *)(row + sizeof(*row_header));
		row_header->row_bytes = row_bytes;
		row_header->column_count = projection_runtime.config.column_count;
		data_offset = sizeof(*row_header) +
			projection_runtime.config.column_count * sizeof(*columns);

		for(uint32_t col = 0; col < projection_runtime.config.column_count; col++)
		{
			uint32_t attr = projection_runtime.config.attr_index[col];
			filter_tuple_value_t *value = &values[attr];

			columns[col].flags = value->flags;
			columns[col].data_length = value->length;
			if((value->flags & FILTER_PROJECTION_COLUMN_FLAG_NULL) == 0U)
			{
				columns[col].data_offset = data_offset;
				if(value->length != 0U)
					memcpy(row + data_offset, value->data, value->length);
				data_offset += projection_align4(value->length);
			}
		}

		projection_runtime.header->row_count++;
		rows_written++;
	}

	projection_record_mode(FILTER_PROJECTION_EFFECTIVE_ARM, rows_written);
	if(rows_written_out)
		*rows_written_out = rows_written;
	return 0;
}

static int projection_validate_fpga_rows(const unsigned char *staging,
										 uint32_t staging_bytes,
										 uint32_t row_count,
										 uint32_t *total_bytes)
{
	uint32_t offset = 0;

	for(uint32_t row_index = 0; row_index < row_count; row_index++)
	{
		const filter_projection_row_header_t *row;
		const filter_projection_column_t *columns;

		if(offset > staging_bytes || sizeof(*row) > staging_bytes - offset)
			return -1;
		row = (const filter_projection_row_header_t *)(staging + offset);
		if(row->column_count != projection_runtime.config.column_count ||
		   row->row_bytes < sizeof(*row) +
			row->column_count * sizeof(filter_projection_column_t) ||
		   (row->row_bytes & 3U) != 0U ||
		   row->row_bytes > projection_runtime.config.max_row_bytes ||
		   row->row_bytes > staging_bytes - offset)
			return -1;

		columns = (const filter_projection_column_t *)
			((const unsigned char *)row + sizeof(*row));
		for(uint32_t col = 0; col < row->column_count; col++)
		{
			if((columns[col].flags & ~FILTER_PROJECTION_COLUMN_FLAG_NULL) != 0U)
				return -1;
			if((columns[col].flags & FILTER_PROJECTION_COLUMN_FLAG_NULL) != 0U)
			{
				if(columns[col].data_length != 0U || columns[col].data_offset != 0U)
					return -1;
			}
			else if(columns[col].data_offset < sizeof(*row) +
					 row->column_count * sizeof(filter_projection_column_t) ||
					(columns[col].data_offset & 3U) != 0U ||
					columns[col].data_offset > row->row_bytes ||
					columns[col].data_length > row->row_bytes - columns[col].data_offset)
			{
				return -1;
			}
		}
		offset += row->row_bytes;
	}

	*total_bytes = offset;
	return 0;
}

int filter_projection_append_fpga(const void *staging_buffer,
								  uint32_t staging_bytes,
								  uint32_t row_count)
{
	const unsigned char *staging = (const unsigned char *)staging_buffer;
	unsigned char *destination;
	uint32_t total_bytes;

	if(!projection_runtime.header || !staging ||
	   projection_runtime.config.mode != FILTER_PROJECTION_MODE_FPGA ||
	   row_count > MAX_TUPLE_PER_PAGE ||
	   projection_validate_fpga_rows(staging, staging_bytes, row_count,
								 &total_bytes) < 0)
	{
		projection_set_error(FILTER_PROJECTION_ERROR_BAD_FPGA_STAGE,
						 FILTER_PROJECTION_RESULT_FLAG_INVALID_STAGE);
		return -2;
	}

	if(total_bytes != 0U)
	{
		if(projection_runtime.write_offset > projection_runtime.result_bytes ||
		   total_bytes > projection_runtime.result_bytes - projection_runtime.write_offset)
		{
			projection_set_error(FILTER_PROJECTION_ERROR_RESULT_FULL,
							 FILTER_PROJECTION_RESULT_FLAG_FULL);
			projection_runtime.header->dropped_rows += row_count;
			return -1;
		}
		destination = projection_runtime.result + projection_runtime.write_offset;
		memcpy(destination, staging, total_bytes);
		projection_runtime.write_offset += total_bytes;
		projection_runtime.header->bytes_used = projection_runtime.write_offset;
	}

	projection_runtime.header->row_count += row_count;
	projection_record_mode(FILTER_PROJECTION_EFFECTIVE_FPGA, row_count);
	{
		uint32_t pages = filter_projection_result_pages();
		memcpy(projection_runtime.result, &pages, sizeof(pages));
	}
	return 0;
}

uint32_t filter_projection_result_pages(void)
{
	uint32_t pages;

	if(!projection_runtime.header)
		return 1U;
	pages = (projection_runtime.write_offset + PG_PAGE_SIZE - 1U) / PG_PAGE_SIZE;
	return pages ? pages : 1U;
}
