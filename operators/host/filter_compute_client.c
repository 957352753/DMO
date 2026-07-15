#include "filter_compute_client.h"

#include <string.h>

static int filter_compute_mode_valid(uint32_t mode)
{
	return mode == FILTER_COMPUTE_MODE_ARM || mode == FILTER_COMPUTE_MODE_FPGA ||
		   mode == FILTER_COMPUTE_MODE_AUTO;
}

int filter_compute_build_aggregate_metadata(void *qual_metadata_page,
										size_t page_bytes,
										uint8_t mode,
										const filter_compute_op_desc_t *ops,
										uint8_t op_count)
{
	filter_compute_desc_t *desc;

	if(!qual_metadata_page || !ops || !filter_compute_mode_valid(mode) ||
	   op_count == 0U || op_count > FILTER_COMPUTE_MAX_OPS ||
	   page_bytes < FILTER_COMPUTE_DESC_OFFSET + sizeof(*desc))
		return -1;
	for(uint32_t i = 0; i < op_count; i++)
	{
		if((ops[i].opcode != FILTER_COMPUTE_OP_SUM &&
			ops[i].opcode != FILTER_COMPUTE_OP_MIN &&
			ops[i].opcode != FILTER_COMPUTE_OP_MAX) ||
		   (ops[i].value_type < FILTER_VALUE_TYPE_INT32 ||
			ops[i].value_type > FILTER_VALUE_TYPE_FLOAT64) ||
		   ops[i].attr_index >= 16U || ops[i].flags != 0U)
			return -1;
	}
	desc = (filter_compute_desc_t *)
		((uint8_t *)qual_metadata_page + FILTER_COMPUTE_DESC_OFFSET);
	memset(desc, 0, sizeof(*desc));
	desc->magic = FILTER_COMPUTE_DESC_MAGIC;
	desc->version = FILTER_COMPUTE_DESC_VERSION;
	desc->struct_bytes = sizeof(*desc);
	desc->mode = mode;
	desc->kind = FILTER_COMPUTE_KIND_AGGREGATE;
	desc->op_count = op_count;
	memcpy(desc->ops, ops, (size_t)op_count * sizeof(*ops));
	return 0;
}

int filter_compute_build_sort_metadata(void *qual_metadata_page,
								   size_t page_bytes,
								   uint8_t mode,
								   uint8_t attr_index,
								   uint8_t value_type,
								   uint8_t sort_flags,
								   uint32_t sort_limit,
								   uint32_t max_tuple_bytes)
{
	filter_compute_desc_t *desc;

	if(!qual_metadata_page || !filter_compute_mode_valid(mode) ||
	   page_bytes < FILTER_COMPUTE_DESC_OFFSET + sizeof(*desc) ||
	   attr_index >= 16U || value_type < FILTER_VALUE_TYPE_INT32 ||
	   value_type > FILTER_VALUE_TYPE_BYTES ||
	   (sort_flags & ~(FILTER_COMPUTE_OP_FLAG_DESC |
					 FILTER_COMPUTE_OP_FLAG_NULLS_FIRST)) != 0U ||
	   sort_limit == 0U || max_tuple_bytes < 23U || max_tuple_bytes > 8192U)
		return -1;
	desc = (filter_compute_desc_t *)
		((uint8_t *)qual_metadata_page + FILTER_COMPUTE_DESC_OFFSET);
	memset(desc, 0, sizeof(*desc));
	desc->magic = FILTER_COMPUTE_DESC_MAGIC;
	desc->version = FILTER_COMPUTE_DESC_VERSION;
	desc->struct_bytes = sizeof(*desc);
	desc->mode = mode;
	desc->kind = FILTER_COMPUTE_KIND_SORT;
	desc->op_count = 1U;
	desc->sort_limit = sort_limit;
	desc->max_tuple_bytes = max_tuple_bytes;
	desc->ops[0].opcode = FILTER_COMPUTE_OP_SORT;
	desc->ops[0].value_type = value_type;
	desc->ops[0].attr_index = attr_index;
	desc->ops[0].flags = sort_flags;
	return 0;
}

int filter_compute_result_open(filter_compute_result_view_t *view,
							   const void *result_buffer,
							   size_t result_bytes)
{
	const uint8_t *buffer = (const uint8_t *)result_buffer;
	const filter_compute_result_header_t *header;
	uint32_t dma_pages;

	if(!view || !buffer || result_bytes < FILTER_COMPUTE_RESULT_DATA_OFFSET)
		return -1;
	memcpy(&dma_pages, buffer, sizeof(dma_pages));
	if(dma_pages == 0U || (size_t)dma_pages * 8192U > result_bytes)
		return -1;
	header = (const filter_compute_result_header_t *)
		(buffer + FILTER_COMPUTE_RESULT_HEADER_OFFSET);
	if(header->magic != FILTER_COMPUTE_RESULT_MAGIC ||
	   header->version != FILTER_COMPUTE_RESULT_VERSION ||
	   header->header_bytes != sizeof(*header) ||
	   (header->kind != FILTER_COMPUTE_KIND_AGGREGATE &&
		header->kind != FILTER_COMPUTE_KIND_SORT) ||
	   header->op_count == 0U || header->op_count > FILTER_COMPUTE_MAX_OPS ||
	   header->bytes_used < FILTER_COMPUTE_RESULT_DATA_OFFSET ||
	   header->bytes_used > result_bytes)
		return -1;
	memset(view, 0, sizeof(*view));
	view->buffer = buffer;
	view->buffer_bytes = header->bytes_used;
	view->header = header;
	view->cursor = FILTER_COMPUTE_RESULT_DATA_OFFSET;
	return 0;
}

int filter_compute_aggregate_values(const filter_compute_result_view_t *view,
									const filter_compute_aggregate_value_t **values,
									uint32_t *value_count)
{
	size_t bytes;

	if(!view || !view->header || !values || !value_count ||
	   view->header->kind != FILTER_COMPUTE_KIND_AGGREGATE)
		return -1;
	bytes = (size_t)view->header->op_count *
		sizeof(filter_compute_aggregate_value_t);
	if(FILTER_COMPUTE_RESULT_DATA_OFFSET + bytes != view->buffer_bytes)
		return -1;
	*values = (const filter_compute_aggregate_value_t *)
		(view->buffer + FILTER_COMPUTE_RESULT_DATA_OFFSET);
	*value_count = view->header->op_count;
	return 0;
}

int filter_compute_sort_next(filter_compute_result_view_t *view,
							 const filter_compute_sort_row_t **row_out,
							 const void **tuple_out,
							 const void **key_out)
{
	const filter_compute_sort_row_t *row;
	const uint8_t *tuple;

	if(!view || !view->header || !row_out || !tuple_out || !key_out ||
	   view->header->kind != FILTER_COMPUTE_KIND_SORT)
		return -1;
	if(view->row_index >= view->header->output_rows)
		return view->cursor == view->buffer_bytes ? 0 : -1;
	if(view->cursor > view->buffer_bytes ||
	   sizeof(*row) > view->buffer_bytes - view->cursor)
		return -1;
	row = (const filter_compute_sort_row_t *)(view->buffer + view->cursor);
	if(row->row_bytes < sizeof(*row) + row->tuple_bytes ||
	   (row->row_bytes & 3U) != 0U ||
	   row->row_bytes > view->buffer_bytes - view->cursor ||
	   (row->flags & ~FILTER_COMPUTE_SORT_ROW_FLAG_KEY_NULL) != 0U)
		return -1;
	tuple = (const uint8_t *)row + sizeof(*row);
	if((row->flags & FILTER_COMPUTE_SORT_ROW_FLAG_KEY_NULL) != 0U)
	{
		if(row->key_bytes != 0U)
			return -1;
		*key_out = NULL;
	}
	else
	{
		if(row->key_offset > row->tuple_bytes ||
		   row->key_bytes > row->tuple_bytes - row->key_offset)
			return -1;
		*key_out = tuple + row->key_offset;
	}
	*row_out = row;
	*tuple_out = tuple;
	view->cursor += row->row_bytes;
	view->row_index++;
	return 1;
}
