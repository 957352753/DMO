#ifndef FILTER_COMPUTE_H_
#define FILTER_COMPUTE_H_

#include <stdint.h>

#include "filter_common.h"
#include "filter_compute_abi.h"

void filter_compute_set_workspace(void *workspace, uint32_t workspace_bytes);
void filter_compute_disable(void);

/* Returns 1 when enabled, 0 for legacy metadata, and -1 when malformed. */
int filter_compute_configure(const filter_compute_desc_t *desc,
							 uint32_t natts,
							 uint32_t count_only);
void filter_compute_reset_result(void *result_buffer, uint32_t result_bytes);

uint32_t filter_compute_enabled(void);
uint32_t filter_compute_kind(void);
uint32_t filter_compute_mode(void);
uint32_t filter_compute_fpga_enabled(void);

/* Packed hardware configuration for filter_accel_set_compute(). */
uint32_t filter_compute_fpga_config(void);

int filter_compute_consume_arm(const unsigned char *page_data,
							   const unsigned short *tuple_offs,
							   const unsigned short *tuple_lens,
							   int hit_count,
							   const tupdesc_t *tupdesc,
							   uint32_t natts,
							   uint16_t pd_lower);

int filter_compute_merge_fpga(const void *staging,
							  uint32_t staging_bytes,
							  uint32_t hit_count);

void filter_compute_finalize(void);
uint32_t filter_compute_result_pages(void);

#endif /* FILTER_COMPUTE_H_ */
