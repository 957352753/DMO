#ifndef FILTER_JOIN_CLIENT_H_
#define FILTER_JOIN_CLIENT_H_

#include <stddef.h>
#include <stdint.h>

#include "../ws2/ftl/run-gr3ftl/src/nvme/filter_join_abi.h"

typedef struct filter_join_result_view
{
	const uint8_t *buffer;
	size_t buffer_bytes;
	const filter_join_result_header_t *header;
	size_t cursor;
	uint64_t row_index;
} filter_join_result_view_t;

int filter_join_build_metadata(void *qual_metadata_page,
						   size_t page_bytes,
						   const filter_join_desc_t *config);

int filter_join_result_open(filter_join_result_view_t *view,
							const void *result_buffer,
							size_t result_bytes);

/* Returns 1 for a match, 0 at end, and -1 for malformed data. */
int filter_join_result_next(filter_join_result_view_t *view,
							const filter_join_row_t **row,
							const void **build_tuple,
							const void **probe_tuple);

#endif /* FILTER_JOIN_CLIENT_H_ */
