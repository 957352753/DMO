#ifndef FILTER_PROJECTION_CLIENT_H_
#define FILTER_PROJECTION_CLIENT_H_

#include <stddef.h>
#include <stdint.h>

#include "../ws2/ftl/run-gr3ftl/src/nvme/filter_projection_abi.h"

typedef struct filter_projection_result_view
{
	const uint8_t *buffer;
	size_t buffer_bytes;
	const filter_projection_result_header_t *header;
	size_t cursor;
	uint32_t row_index;
} filter_projection_result_view_t;

/* Attribute indexes are zero-based and output order is preserved. */
int filter_projection_build_metadata(void *qual_metadata_page,
								 size_t page_bytes,
								 uint8_t mode,
								 const uint8_t *attr_index,
								 uint8_t column_count,
								 uint32_t max_row_bytes);

int filter_projection_result_open(filter_projection_result_view_t *view,
								  const void *result_buffer,
								  size_t result_bytes);

/* Returns 1 for a row, 0 at end of input, and -1 for malformed data. */
int filter_projection_result_next(filter_projection_result_view_t *view,
								  const filter_projection_row_header_t **row,
								  const filter_projection_column_t **columns);

int filter_projection_column_data(const filter_projection_row_header_t *row,
								  const filter_projection_column_t *column,
								  const void **data,
								  uint16_t *length,
								  uint16_t *flags);

#endif /* FILTER_PROJECTION_CLIENT_H_ */
