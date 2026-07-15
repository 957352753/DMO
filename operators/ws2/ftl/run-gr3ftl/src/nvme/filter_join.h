#ifndef FILTER_JOIN_H_
#define FILTER_JOIN_H_

#include <stdint.h>

#include "filter_common.h"
#include "filter_join_abi.h"

void filter_join_set_workspace(void *workspace, uint32_t workspace_bytes);
void filter_join_disable(void);
void filter_join_reset_session(void);

/* BUILD initializes or extends a persistent session. PROBE requires the same
 * session_id and matching build-key metadata. */
int filter_join_configure(const filter_join_desc_t *desc,
						  uint32_t natts,
						  uint32_t count_only);
void filter_join_reset_result(void *result_buffer, uint32_t result_bytes);

uint32_t filter_join_enabled(void);
uint32_t filter_join_phase(void);
uint32_t filter_join_fpga_enabled(void);
uint32_t filter_join_fpga_config(void);
uint32_t filter_join_hash_seed(void);

int filter_join_consume_arm(const unsigned char *page_data,
							const unsigned short *tuple_offs,
							const unsigned short *tuple_lens,
							int hit_count,
							const tupdesc_t *tupdesc,
							uint32_t natts,
							uint16_t pd_lower);

int filter_join_consume_fpga_hashes(const void *staging,
								uint32_t staging_bytes,
								const unsigned char *page_data,
								const unsigned short *tuple_offs,
								const unsigned short *tuple_lens,
								int hit_count,
								const tupdesc_t *tupdesc,
								uint32_t natts,
								uint16_t pd_lower);

void filter_join_finalize(void);
uint32_t filter_join_result_pages(void);

#endif /* FILTER_JOIN_H_ */
