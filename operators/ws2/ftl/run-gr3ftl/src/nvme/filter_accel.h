#ifndef __FILTER_ACCEL_H__
#define __FILTER_ACCEL_H__

#include <stdint.h>
#include "../filter_config.h"

#define FILTER_ACCEL_STATUS_DONE         0x00000001U
#define FILTER_ACCEL_STATUS_BUSY         0x00000002U
#define FILTER_ACCEL_STATUS_UNSUPPORTED  0x00000004U
#define FILTER_ACCEL_STATUS_ERROR        0x00000008U

#define FILTER_ACCEL_OK                  0
#define FILTER_ACCEL_ERR_UNSUPPORTED    -1
#define FILTER_ACCEL_ERR_TIMEOUT        -2
#define FILTER_ACCEL_ERR_HW             -3
#define FILTER_ACCEL_PENDING             1

#define FILTER_ACCEL_HIT_TUPLE_OFF(desc) ((uint16_t)((desc) & 0xffffU))
#define FILTER_ACCEL_HIT_TUPLE_LEN(desc) ((uint16_t)(((desc) >> 16) & 0xffffU))

#define FILTER_ACCEL_BATCH_COUNT_OFFSET  0x000U
#define FILTER_ACCEL_BATCH_ADDR_OFFSET   0x100U
#define FILTER_ACCEL_BATCH_PROJECTION_BYTES_OFFSET FILTER_ACCEL_BATCH_ADDR_OFFSET
#define FILTER_ACCEL_BATCH_DESC_OFFSET   0x200U
#define FILTER_ACCEL_BATCH_HIT_STRIDE    0x400U

#if (FILTER_ACCEL_MAX_BATCH_PAGES * 4U) > FILTER_ACCEL_BATCH_ADDR_OFFSET
#error "accelerator tuple-count table overlaps the address table"
#endif

#if (FILTER_ACCEL_BATCH_ADDR_OFFSET + FILTER_ACCEL_MAX_BATCH_PAGES * 4U) > \
    FILTER_ACCEL_BATCH_DESC_OFFSET
#error "accelerator address table overlaps the result descriptors"
#endif

typedef void (*filter_accel_poll_hook_t)(void);

int filter_accel_run_page(uint32_t page_addr,
                          uint32_t tupdesc_addr,
                          uint32_t filter_addr,
                          uint32_t qual_addr,
                          uint32_t hit_addr,
                          uint32_t page_id,
                          uint32_t natts,
                          uint32_t filter_cnt,
                          uint32_t tuple_cnt,
                          uint32_t *hit_count_out);

int filter_accel_run_batch(uint32_t page_addr,
                           uint32_t tupdesc_addr,
                           uint32_t filter_addr,
                           uint32_t qual_addr,
                           uint32_t hit_addr,
                           uint32_t page_id,
                           uint32_t natts,
                           uint32_t filter_cnt,
                           const uint16_t *tuple_counts,
                           uint32_t page_count,
                           uint32_t page_stride,
                           uint32_t hit_stride);

int filter_accel_run_batch_sg(const uint32_t *page_addrs,
                              uint32_t tupdesc_addr,
                              uint32_t filter_addr,
                              uint32_t qual_addr,
                              uint32_t hit_addr,
                              uint32_t page_id,
                              uint32_t natts,
                              uint32_t filter_cnt,
                              const uint16_t *tuple_counts,
                              uint32_t page_count,
                              uint32_t hit_stride);

int filter_accel_start_batch_sg(const uint32_t *page_addrs,
                                uint32_t tupdesc_addr,
                                uint32_t filter_addr,
                                uint32_t qual_addr,
                                uint32_t hit_addr,
                                uint32_t page_id,
                                uint32_t natts,
                                uint32_t filter_cnt,
                                const uint16_t *tuple_counts,
                                uint32_t page_count,
                                uint32_t hit_stride);

int filter_accel_poll_batch_sg(uint32_t hit_addr,
                               uint32_t page_count,
                               uint32_t hit_stride);

uint32_t filter_accel_read_status(void);
uint32_t filter_accel_read_error(void);
void filter_accel_reset_cache(void);
void filter_accel_set_count_only(uint32_t enable);
void filter_accel_set_projection(uint32_t enable,
                                 uint32_t output_addr,
                                 uint32_t output_stride,
                                 uint32_t column_count,
                                 uint32_t max_row_bytes,
                                 const uint8_t *attr_index);
void filter_accel_set_compute(uint32_t enable,
                              uint32_t output_addr,
                              uint32_t output_stride,
                              uint32_t config,
                              uint32_t hash_seed);
void filter_accel_set_poll_hook(filter_accel_poll_hook_t hook, uint32_t interval);

#endif
