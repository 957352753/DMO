#include "filter_projection_client.h"

#include <string.h>

#define FILTER_PROJECTION_HOST_DEFAULT_MAX_ROW_BYTES 4096U

int filter_projection_build_metadata(void *qual_metadata_page,
								 size_t page_bytes,
								 uint8_t mode,
								 const uint8_t *attr_index,
								 uint8_t column_count,
								 uint32_t max_row_bytes)
{
	filter_projection_desc_t *desc;
	uint32_t seen = 0;

	if(!qual_metadata_page || !attr_index ||
	   page_bytes < FILTER_PROJECTION_DESC_OFFSET + sizeof(*desc) ||
	   (mode != FILTER_PROJECTION_MODE_ARM &&
		mode != FILTER_PROJECTION_MODE_FPGA) ||
	   column_count == 0U || column_count > FILTER_PROJECTION_MAX_COLUMNS)
		return -1;

	for(uint32_t i = 0; i < column_count; i++)
	{
		if(attr_index[i] >= FILTER_PROJECTION_MAX_COLUMNS ||
		   (seen & (1U << attr_index[i])) != 0U)
			return -1;
		seen |= 1U << attr_index[i];
	}

	desc = (filter_projection_desc_t *)
		((uint8_t *)qual_metadata_page + FILTER_PROJECTION_DESC_OFFSET);
	memset(desc, 0, sizeof(*desc));
	desc->magic = FILTER_PROJECTION_DESC_MAGIC;
	desc->version = FILTER_PROJECTION_DESC_VERSION;
	desc->struct_bytes = sizeof(*desc);
	desc->mode = mode;
	desc->output_format = FILTER_PROJECTION_FORMAT_COMPACT_V1;
	desc->column_count = column_count;
	desc->max_row_bytes = max_row_bytes ? max_row_bytes :
		FILTER_PROJECTION_HOST_DEFAULT_MAX_ROW_BYTES;
	memcpy(desc->attr_index, attr_index, column_count);
	return 0;
}

int filter_projection_result_open(filter_projection_result_view_t *view,
								  const void *result_buffer,
								  size_t result_bytes)
{
	const uint8_t *buffer = (const uint8_t *)result_buffer;
	const filter_projection_result_header_t *header;
	uint32_t dma_pages;

	if(!view || !buffer || result_bytes < FILTER_PROJECTION_RESULT_DATA_OFFSET)
		return -1;
	memcpy(&dma_pages, buffer, sizeof(dma_pages));
	if(dma_pages == 0U || (size_t)dma_pages * 8192U > result_bytes)
		return -1;

	header = (const filter_projection_result_header_t *)
		(buffer + FILTER_PROJECTION_RESULT_HEADER_OFFSET);
	if(header->magic != FILTER_PROJECTION_RESULT_MAGIC ||
	   header->version != FILTER_PROJECTION_RESULT_VERSION ||
	   header->header_bytes != sizeof(*header) ||
	   header->output_format != FILTER_PROJECTION_FORMAT_COMPACT_V1 ||
	   header->column_count == 0U ||
	   header->column_count > FILTER_PROJECTION_MAX_COLUMNS ||
	   header->bytes_used < FILTER_PROJECTION_RESULT_DATA_OFFSET ||
	   header->bytes_used > result_bytes)
		return -1;

	memset(view, 0, sizeof(*view));
	view->buffer = buffer;
	view->buffer_bytes = header->bytes_used;
	view->header = header;
	view->cursor = FILTER_PROJECTION_RESULT_DATA_OFFSET;
	return 0;
}

int filter_projection_result_next(filter_projection_result_view_t *view,
								  const filter_projection_row_header_t **row_out,
								  const filter_projection_column_t **columns_out)
{
	const filter_projection_row_header_t *row;
	size_t minimum_row_bytes;

	if(!view || !view->header || !row_out || !columns_out)
		return -1;
	if(view->row_index >= view->header->row_count)
		return view->cursor == view->buffer_bytes ? 0 : -1;
	if(view->cursor > view->buffer_bytes ||
	   sizeof(*row) > view->buffer_bytes - view->cursor)
		return -1;

	row = (const filter_projection_row_header_t *)(view->buffer + view->cursor);
	minimum_row_bytes = sizeof(*row) +
		(size_t)view->header->column_count * sizeof(filter_projection_column_t);
	if(row->column_count != view->header->column_count ||
	   row->row_bytes < minimum_row_bytes || (row->row_bytes & 3U) != 0U ||
	   row->row_bytes > view->buffer_bytes - view->cursor)
		return -1;

	*row_out = row;
	*columns_out = (const filter_projection_column_t *)
		((const uint8_t *)row + sizeof(*row));
	view->cursor += row->row_bytes;
	view->row_index++;
	return 1;
}

int filter_projection_column_data(const filter_projection_row_header_t *row,
								  const filter_projection_column_t *column,
								  const void **data,
								  uint16_t *length,
								  uint16_t *flags)
{
	if(!row || !column || !data || !length || !flags ||
	   (column->flags & ~FILTER_PROJECTION_COLUMN_FLAG_NULL) != 0U)
		return -1;

	*length = column->data_length;
	*flags = column->flags;
	if((column->flags & FILTER_PROJECTION_COLUMN_FLAG_NULL) != 0U)
	{
		if(column->data_offset != 0U || column->data_length != 0U)
			return -1;
		*data = 0;
		return 0;
	}

	if(column->data_offset < sizeof(*row) +
			row->column_count * sizeof(filter_projection_column_t) ||
	   column->data_offset > row->row_bytes ||
	   column->data_length > row->row_bytes - column->data_offset)
		return -1;
	*data = (const uint8_t *)row + column->data_offset;
	return 0;
}
