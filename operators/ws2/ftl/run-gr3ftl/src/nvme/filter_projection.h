#ifndef FILTER_PROJECTION_H_
#define FILTER_PROJECTION_H_

#include <stdint.h>

#include "filter_common.h"
#include "filter_projection_abi.h"

/* Returns 1 for an enabled descriptor, 0 for the backward-compatible legacy
 * mode, and a negative value for a malformed descriptor. */
int filter_projection_configure(const filter_projection_desc_t *desc,
								uint32_t natts,
								uint32_t count_only);
void filter_projection_disable(void);
void filter_projection_reset_result(void *result_buffer, uint32_t result_bytes);

uint32_t filter_projection_enabled(void);
uint32_t filter_projection_mode(void);
uint32_t filter_projection_column_count(void);
uint32_t filter_projection_max_row_bytes(void);
const uint8_t *filter_projection_attr_index(void);

/* Returns 0 on success and -1 when the result is full or a tuple is invalid. */
int filter_projection_append_arm(const unsigned char *page_data,
								 const unsigned short *tuple_offs,
								 const unsigned short *tuple_lens,
								 int hit_count,
								 const tupdesc_t *tupdesc,
								 uint32_t natts,
								 uint16_t pd_lower,
								 uint32_t *rows_written_out);

/* Returns 0 on success, -1 when the final result is full, and -2 when FPGA
 * staging is malformed and the caller must rematerialize the page on ARM. */
int filter_projection_append_fpga(const void *staging,
								  uint32_t staging_bytes,
								  uint32_t row_count);

uint32_t filter_projection_result_pages(void);

#endif /* FILTER_PROJECTION_H_ */
