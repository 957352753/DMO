#include "filter_join_client.h"

#include <string.h>

static size_t filter_join_align4(size_t value)
{
	return (value + 3U) & ~(size_t)3U;
}

int filter_join_build_metadata(void *qual_metadata_page,
						   size_t page_bytes,
						   const filter_join_desc_t *config)
{
	filter_join_desc_t *desc;

	if(!qual_metadata_page || !config ||
	   page_bytes < FILTER_JOIN_DESC_OFFSET + sizeof(*desc) ||
	   (config->mode != FILTER_JOIN_MODE_ARM &&
		config->mode != FILTER_JOIN_MODE_HYBRID &&
		config->mode != FILTER_JOIN_MODE_AUTO) ||
	   (config->phase != FILTER_JOIN_PHASE_BUILD &&
		config->phase != FILTER_JOIN_PHASE_PROBE) ||
	   config->key_count == 0U || config->key_count > FILTER_JOIN_MAX_KEYS)
		return -1;
	desc = (filter_join_desc_t *)
		((uint8_t *)qual_metadata_page + FILTER_JOIN_DESC_OFFSET);
	*desc = *config;
	desc->magic = FILTER_JOIN_DESC_MAGIC;
	desc->version = FILTER_JOIN_DESC_VERSION;
	desc->struct_bytes = sizeof(*desc);
	return 0;
}

int filter_join_result_open(filter_join_result_view_t *view,
							const void *result_buffer,
							size_t result_bytes)
{
	const uint8_t *buffer = (const uint8_t *)result_buffer;
	const filter_join_result_header_t *header;
	uint32_t dma_pages;

	if(!view || !buffer || result_bytes < FILTER_JOIN_RESULT_DATA_OFFSET)
		return -1;
	memcpy(&dma_pages, buffer, sizeof(dma_pages));
	if(dma_pages == 0U || (size_t)dma_pages * 8192U > result_bytes)
		return -1;
	header = (const filter_join_result_header_t *)
		(buffer + FILTER_JOIN_RESULT_HEADER_OFFSET);
	if(header->magic != FILTER_JOIN_RESULT_MAGIC ||
	   header->version != FILTER_JOIN_RESULT_VERSION ||
	   header->header_bytes != sizeof(*header) ||
	   (header->phase != FILTER_JOIN_PHASE_BUILD &&
		header->phase != FILTER_JOIN_PHASE_PROBE) ||
	   header->key_count == 0U || header->key_count > FILTER_JOIN_MAX_KEYS ||
	   header->bytes_used < FILTER_JOIN_RESULT_DATA_OFFSET ||
	   header->bytes_used > result_bytes)
		return -1;
	memset(view, 0, sizeof(*view));
	view->buffer = buffer;
	view->buffer_bytes = header->bytes_used;
	view->header = header;
	view->cursor = FILTER_JOIN_RESULT_DATA_OFFSET;
	return 0;
}

int filter_join_result_next(filter_join_result_view_t *view,
							const filter_join_row_t **row_out,
							const void **build_tuple,
							const void **probe_tuple)
{
	const filter_join_row_t *row;
	size_t probe_offset;

	if(!view || !view->header || !row_out || !build_tuple || !probe_tuple ||
	   view->header->phase != FILTER_JOIN_PHASE_PROBE)
		return -1;
	if(view->row_index >= view->header->output_rows)
		return view->cursor == view->buffer_bytes ? 0 : -1;
	if(view->cursor > view->buffer_bytes ||
	   sizeof(*row) > view->buffer_bytes - view->cursor)
		return -1;
	row = (const filter_join_row_t *)(view->buffer + view->cursor);
	probe_offset = filter_join_align4(sizeof(*row) + row->build_tuple_bytes);
	if(row->row_bytes < probe_offset + row->probe_tuple_bytes ||
	   (row->row_bytes & 3U) != 0U ||
	   row->row_bytes > view->buffer_bytes - view->cursor)
		return -1;
	*row_out = row;
	*build_tuple = (const uint8_t *)row + sizeof(*row);
	*probe_tuple = (const uint8_t *)row + probe_offset;
	view->cursor += row->row_bytes;
	view->row_index++;
	return 1;
}
