#ifndef FILTER_COMPUTE_CLIENT_H_
#define FILTER_COMPUTE_CLIENT_H_

#include <stddef.h>
#include <stdint.h>

#include "../ws2/ftl/run-gr3ftl/src/nvme/filter_compute_abi.h"

typedef struct filter_compute_result_view
{
	const uint8_t *buffer;
	size_t buffer_bytes;
	const filter_compute_result_header_t *header;
	size_t cursor;
	uint64_t row_index;
} filter_compute_result_view_t;

int filter_compute_build_aggregate_metadata(void *qual_metadata_page,
										size_t page_bytes,
										uint8_t mode,
										const filter_compute_op_desc_t *ops,
										uint8_t op_count);

int filter_compute_build_sort_metadata(void *qual_metadata_page,
								   size_t page_bytes,
								   uint8_t mode,
								   uint8_t attr_index,
								   uint8_t value_type,
								   uint8_t sort_flags,
								   uint32_t sort_limit,
								   uint32_t max_tuple_bytes);

int filter_compute_result_open(filter_compute_result_view_t *view,
							   const void *result_buffer,
							   size_t result_bytes);

int filter_compute_aggregate_values(const filter_compute_result_view_t *view,
									const filter_compute_aggregate_value_t **values,
									uint32_t *value_count);

/* Returns 1 for a row, 0 at end, and -1 for malformed data. */
int filter_compute_sort_next(filter_compute_result_view_t *view,
							 const filter_compute_sort_row_t **row,
							 const void **tuple,
							 const void **key);

#endif /* FILTER_COMPUTE_CLIENT_H_ */
