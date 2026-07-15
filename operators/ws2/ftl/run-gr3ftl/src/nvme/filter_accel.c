#include "filter_accel.h"
#include "../memory_map.h"
#include "xil_cache.h"
#include "xil_io.h"

#define FILTER_ACCEL_REG_CONTROL        0x00U
#define FILTER_ACCEL_REG_STATUS         0x04U
#define FILTER_ACCEL_REG_PAGE_ADDR      0x08U
#define FILTER_ACCEL_REG_TUPDESC_ADDR   0x0cU
#define FILTER_ACCEL_REG_FILTER_ADDR    0x10U
#define FILTER_ACCEL_REG_QUAL_ADDR      0x14U
#define FILTER_ACCEL_REG_HIT_ADDR       0x18U
#define FILTER_ACCEL_REG_PAGE_ID        0x1cU
#define FILTER_ACCEL_REG_NATTS          0x20U
#define FILTER_ACCEL_REG_FILTER_CNT     0x24U
#define FILTER_ACCEL_REG_TUPLE_CNT      0x28U
#define FILTER_ACCEL_REG_HIT_COUNT      0x2cU
#define FILTER_ACCEL_REG_ERROR          0x30U
#define FILTER_ACCEL_REG_VERSION        0x34U
#define FILTER_ACCEL_REG_PAGE_STRIDE    0x38U
#define FILTER_ACCEL_REG_PAGE_COUNT     0x3cU
#define FILTER_ACCEL_REG_HIT_STRIDE     0x40U
#define FILTER_ACCEL_REG_TUPLE_CNT_ADDR 0x44U
#define FILTER_ACCEL_REG_PAGE_DONE      0x48U
#define FILTER_ACCEL_REG_PAGE_ADDR_TABLE_ADDR 0x4cU
#define FILTER_ACCEL_REG_RESULT_MODE    0x50U
#define FILTER_ACCEL_REG_PROJECTION_ADDR   0x54U
#define FILTER_ACCEL_REG_PROJECTION_STRIDE 0x58U
#define FILTER_ACCEL_REG_PROJECTION_CONFIG 0x5cU
#define FILTER_ACCEL_REG_PROJECTION_COLS0  0x60U
#define FILTER_ACCEL_REG_PROJECTION_COLS1  0x64U
#define FILTER_ACCEL_REG_PROJECTION_COLS2  0x68U
#define FILTER_ACCEL_REG_PROJECTION_COLS3  0x6cU
#define FILTER_ACCEL_REG_COMPUTE_ADDR       0x70U
#define FILTER_ACCEL_REG_COMPUTE_STRIDE     0x74U
#define FILTER_ACCEL_REG_COMPUTE_CONFIG     0x78U
#define FILTER_ACCEL_REG_COMPUTE_SEED       0x7cU

#define FILTER_ACCEL_CONTROL_START      0x00000001U
#define FILTER_ACCEL_CONTROL_CLEAR      0x00000002U
#define FILTER_ACCEL_TIMEOUT_POLLS      100000U
#define FILTER_ACCEL_CACHE_LINE_SIZE    64U
#define FILTER_ACCEL_PAGE_BYTES         8192U

static uint32_t cached_tupdesc_addr;
static uint32_t cached_filter_addr;
static uint32_t cached_qual_addr;
static uint32_t cached_hit_addr;
static uint32_t cached_natts;
static uint32_t cached_filter_cnt;
static uint32_t cached_page_stride;
static uint32_t cached_hit_stride;
static uint32_t cached_tuple_cnt_addr;
static uint32_t cached_page_addr_table_addr;
static uint32_t cached_result_mode;
static uint32_t cached_projection_addr;
static uint32_t cached_projection_stride;
static uint32_t cached_projection_config;
static uint32_t cached_projection_cols[4];
static uint32_t cached_compute_addr;
static uint32_t cached_compute_stride;
static uint32_t cached_compute_config;
static uint32_t cached_compute_seed;
static uint32_t cache_valid;
static uint32_t count_only_enabled;
static uint32_t projection_enabled;
static uint32_t projection_addr;
static uint32_t projection_stride;
static uint32_t projection_config;
static uint32_t projection_cols[4];
static uint32_t compute_enabled;
static uint32_t compute_addr;
static uint32_t compute_stride;
static uint32_t compute_config;
static uint32_t compute_seed;
static filter_accel_poll_hook_t poll_hook;
static uint32_t poll_hook_interval;
static uint32_t poll_hook_countdown;

static void filter_accel_flush_range(uint32_t addr, uint32_t len)
{
    uint32_t start = addr & ~(FILTER_ACCEL_CACHE_LINE_SIZE - 1U);
    uint32_t end = (addr + len + FILTER_ACCEL_CACHE_LINE_SIZE - 1U) &
                   ~(FILTER_ACCEL_CACHE_LINE_SIZE - 1U);

    for (uint32_t line = start; line < end; line += FILTER_ACCEL_CACHE_LINE_SIZE)
        Xil_DCacheFlushLine((INTPTR)line);
}

#if FILTER_ACCEL_INPUT_CACHE_OP == 0
#define filter_accel_prepare_input_range(addr, len) \
    do { (void)(addr); (void)(len); } while (0)
#else
static inline void filter_accel_prepare_input_range(uint32_t addr, uint32_t len)
{
#if FILTER_ACCEL_INPUT_CACHE_OP == 1
    filter_accel_flush_range(addr, len);
#else
    Xil_DCacheInvalidateRange(addr, len);
#endif
}
#endif

static inline void filter_accel_write_reg(uint32_t offset, uint32_t value)
{
    Xil_Out32(FILTER_ACCEL_BASE_ADDR + offset, value);
}

static inline void filter_accel_write_cached(uint32_t offset,
                                             uint32_t value,
                                             uint32_t *cached_value)
{
    if (!cache_valid || *cached_value != value) {
        filter_accel_write_reg(offset, value);
        *cached_value = value;
    }
}

static inline uint32_t filter_accel_read_reg(uint32_t offset)
{
    return Xil_In32(FILTER_ACCEL_BASE_ADDR + offset);
}

uint32_t filter_accel_read_status(void)
{
    return filter_accel_read_reg(FILTER_ACCEL_REG_STATUS);
}

uint32_t filter_accel_read_error(void)
{
    return filter_accel_read_reg(FILTER_ACCEL_REG_ERROR);
}

void filter_accel_set_poll_hook(filter_accel_poll_hook_t hook, uint32_t interval)
{
    poll_hook = hook;
    poll_hook_interval = interval;
    poll_hook_countdown = (hook && interval) ? 1U : 0U;
}

static inline void filter_accel_poll_service(void)
{
    if (!poll_hook || !poll_hook_interval)
        return;

    if (poll_hook_countdown > 1U) {
        poll_hook_countdown--;
        return;
    }

    poll_hook_countdown = poll_hook_interval;
    poll_hook();
}

void filter_accel_reset_cache(void)
{
    cache_valid = 0;
}

void filter_accel_set_count_only(uint32_t enable)
{
    count_only_enabled = enable ? 1U : 0U;
}

void filter_accel_set_projection(uint32_t enable,
                                 uint32_t output_addr,
                                 uint32_t output_stride,
                                 uint32_t column_count,
                                 uint32_t max_row_bytes,
                                 const uint8_t *attr_index)
{
    uint32_t scan_natts = 0U;

    projection_enabled = (enable && attr_index && column_count > 0U &&
                          column_count <= 16U) ? 1U : 0U;
    projection_addr = projection_enabled ? output_addr : 0U;
    projection_stride = projection_enabled ? output_stride : 0U;
    if (projection_enabled) {
        for (uint32_t column = 0; column < column_count; column++) {
            uint32_t candidate = (uint32_t)attr_index[column] + 1U;
            if (candidate > scan_natts)
                scan_natts = candidate;
        }
    }
    projection_config = projection_enabled ?
        ((max_row_bytes << 16) | (scan_natts << 8) | column_count) : 0U;

    for (uint32_t word = 0; word < 4U; word++) {
        uint32_t packed = 0U;

        if (projection_enabled) {
            for (uint32_t byte = 0; byte < 4U; byte++) {
                uint32_t index = word * 4U + byte;
                uint32_t attr = (index < column_count) ? attr_index[index] : 0U;
                packed |= attr << (byte * 8U);
            }
        }
        projection_cols[word] = packed;
    }
}

void filter_accel_set_compute(uint32_t enable,
                              uint32_t output_addr,
                              uint32_t output_stride,
                              uint32_t config,
                              uint32_t hash_seed)
{
    uint32_t kind = config & 0x0fU;

    compute_enabled = (enable && output_addr && output_stride >= 64U &&
                       (output_stride & 3U) == 0U &&
                       (kind == 1U || kind == 2U)) ? 1U : 0U;
    compute_addr = compute_enabled ? output_addr : 0U;
    compute_stride = compute_enabled ? output_stride : 0U;
    compute_config = compute_enabled ? config : 0U;
    compute_seed = compute_enabled ? hash_seed : 0U;
}

static void filter_accel_program_projection(void)
{
    uint32_t result_mode = count_only_enabled |
                           (projection_enabled ? 0x00000002U : 0U) |
                           (compute_enabled ? 0x00000004U : 0U);

    filter_accel_write_cached(FILTER_ACCEL_REG_RESULT_MODE,
                              result_mode, &cached_result_mode);
    filter_accel_write_cached(FILTER_ACCEL_REG_PROJECTION_ADDR,
                              projection_addr, &cached_projection_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_PROJECTION_STRIDE,
                              projection_stride, &cached_projection_stride);
    filter_accel_write_cached(FILTER_ACCEL_REG_PROJECTION_CONFIG,
                              projection_config, &cached_projection_config);
    filter_accel_write_cached(FILTER_ACCEL_REG_PROJECTION_COLS0,
                              projection_cols[0], &cached_projection_cols[0]);
    filter_accel_write_cached(FILTER_ACCEL_REG_PROJECTION_COLS1,
                              projection_cols[1], &cached_projection_cols[1]);
    filter_accel_write_cached(FILTER_ACCEL_REG_PROJECTION_COLS2,
                              projection_cols[2], &cached_projection_cols[2]);
    filter_accel_write_cached(FILTER_ACCEL_REG_PROJECTION_COLS3,
                              projection_cols[3], &cached_projection_cols[3]);
    filter_accel_write_cached(FILTER_ACCEL_REG_COMPUTE_ADDR,
                              compute_addr, &cached_compute_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_COMPUTE_STRIDE,
                              compute_stride, &cached_compute_stride);
    filter_accel_write_cached(FILTER_ACCEL_REG_COMPUTE_CONFIG,
                              compute_config, &cached_compute_config);
    filter_accel_write_cached(FILTER_ACCEL_REG_COMPUTE_SEED,
                              compute_seed, &cached_compute_seed);
}

static void filter_accel_invalidate_projection(uint32_t hit_addr,
                                               uint32_t page_count)
{
    if (!projection_enabled || page_count == 0U)
        return;

    if (page_count == 1U) {
        Xil_DCacheInvalidateRange(projection_addr, projection_stride);
        return;
    }

    volatile uint32_t *bytes_table = (volatile uint32_t *)(uintptr_t)
        (hit_addr + FILTER_ACCEL_BATCH_PROJECTION_BYTES_OFFSET);
    for (uint32_t page = 0; page < page_count; page++) {
        uint32_t bytes = bytes_table[page];

        if (bytes > projection_stride)
            bytes = projection_stride;
        if (bytes != 0U)
            Xil_DCacheInvalidateRange(projection_addr + page * projection_stride,
                                      bytes);
    }
}

static void filter_accel_invalidate_compute(uint32_t hit_addr,
                                            uint32_t page_count)
{
    uint32_t kind = compute_config & 0x0fU;

    if (!compute_enabled || page_count == 0U)
        return;
    for (uint32_t page = 0; page < page_count; page++) {
        uint32_t bytes = 64U;

        if (kind == 2U) {
            if (page_count == 1U) {
                bytes = 16U + 128U * sizeof(uint32_t);
            } else {
                volatile uint32_t *counts =
                    (volatile uint32_t *)(uintptr_t)hit_addr;
                uint32_t count = counts[page];

                if (count > 128U)
                    count = 128U;
                bytes = 16U + count * sizeof(uint32_t);
            }
        }
        if (bytes > compute_stride)
            bytes = compute_stride;
        Xil_DCacheInvalidateRange(compute_addr + page * compute_stride, bytes);
    }
}

static void filter_accel_start_page_hw(uint32_t page_addr,
                                       uint32_t tupdesc_addr,
                                       uint32_t filter_addr,
                                       uint32_t qual_addr,
                                       uint32_t hit_addr,
                                       uint32_t page_id,
                                       uint32_t natts,
                                       uint32_t filter_cnt,
                                       uint32_t tuple_cnt)
{
    filter_accel_prepare_input_range(page_addr, FILTER_ACCEL_PAGE_BYTES);
    filter_accel_write_reg(FILTER_ACCEL_REG_CONTROL, FILTER_ACCEL_CONTROL_CLEAR);
    filter_accel_write_reg(FILTER_ACCEL_REG_PAGE_ADDR, page_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_TUPDESC_ADDR, tupdesc_addr, &cached_tupdesc_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_FILTER_ADDR, filter_addr, &cached_filter_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_QUAL_ADDR, qual_addr, &cached_qual_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_HIT_ADDR, hit_addr, &cached_hit_addr);
    filter_accel_write_reg(FILTER_ACCEL_REG_PAGE_ID, page_id);
    filter_accel_write_cached(FILTER_ACCEL_REG_NATTS, natts, &cached_natts);
    filter_accel_write_cached(FILTER_ACCEL_REG_FILTER_CNT, filter_cnt, &cached_filter_cnt);
    filter_accel_write_cached(FILTER_ACCEL_REG_PAGE_STRIDE,
                              FILTER_ACCEL_PAGE_BYTES, &cached_page_stride);
    filter_accel_write_cached(FILTER_ACCEL_REG_HIT_STRIDE, 0U, &cached_hit_stride);
    filter_accel_write_cached(FILTER_ACCEL_REG_TUPLE_CNT_ADDR, 0U, &cached_tuple_cnt_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_PAGE_ADDR_TABLE_ADDR,
                              0U, &cached_page_addr_table_addr);
    filter_accel_program_projection();
    filter_accel_write_reg(FILTER_ACCEL_REG_PAGE_COUNT, 1U);
    filter_accel_write_reg(FILTER_ACCEL_REG_TUPLE_CNT, tuple_cnt);
    cache_valid = 1;
    filter_accel_write_reg(FILTER_ACCEL_REG_CONTROL, FILTER_ACCEL_CONTROL_START);
}

static int filter_accel_poll_page_hw(uint32_t hit_addr,
                                     uint32_t *hit_count_out)
{
    uint32_t status = filter_accel_read_reg(FILTER_ACCEL_REG_STATUS);

    if (!(status & FILTER_ACCEL_STATUS_DONE))
        return FILTER_ACCEL_PENDING;
    if (status & FILTER_ACCEL_STATUS_UNSUPPORTED)
        return FILTER_ACCEL_ERR_UNSUPPORTED;
    if (status & FILTER_ACCEL_STATUS_ERROR)
        return FILTER_ACCEL_ERR_HW;

    if (hit_count_out)
        *hit_count_out = filter_accel_read_reg(FILTER_ACCEL_REG_HIT_COUNT);
    if (!count_only_enabled)
        Xil_DCacheInvalidateRange(hit_addr, FILTER_ACCEL_BATCH_HIT_STRIDE);
    filter_accel_invalidate_projection(hit_addr, 1U);
    filter_accel_invalidate_compute(hit_addr, 1U);
    return FILTER_ACCEL_OK;
}

int filter_accel_run_page(uint32_t page_addr,
                          uint32_t tupdesc_addr,
                          uint32_t filter_addr,
                          uint32_t qual_addr,
                          uint32_t hit_addr,
                          uint32_t page_id,
                          uint32_t natts,
                          uint32_t filter_cnt,
                          uint32_t tuple_cnt,
                          uint32_t *hit_count_out)
{
    uint32_t polls;
    int rc;

    filter_accel_start_page_hw(page_addr, tupdesc_addr, filter_addr, qual_addr,
                               hit_addr, page_id, natts, filter_cnt, tuple_cnt);

    for (polls = 0; polls < FILTER_ACCEL_TIMEOUT_POLLS; polls++) {
        rc = filter_accel_poll_page_hw(hit_addr, hit_count_out);
        if (rc != FILTER_ACCEL_PENDING)
            return rc;
        filter_accel_poll_service();
    }

    return FILTER_ACCEL_ERR_TIMEOUT;
}

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
                           uint32_t hit_stride)
{
    volatile uint32_t *count_table =
        (volatile uint32_t *)(uintptr_t)hit_addr;
    uint32_t status;
    uint32_t polls;

    if (!tuple_counts || page_count == 0U)
        return FILTER_ACCEL_ERR_HW;

    if (page_count == 1U) {
        uint32_t hit_count = 0;
        int rc = filter_accel_run_page(page_addr, tupdesc_addr, filter_addr, qual_addr,
                                       hit_addr + FILTER_ACCEL_BATCH_DESC_OFFSET,
                                       page_id, natts, filter_cnt, tuple_counts[0],
                                       &hit_count);
        if (rc == FILTER_ACCEL_OK)
            count_table[0] = hit_count;
        return rc;
    }

    if (page_count > FILTER_ACCEL_MAX_BATCH_PAGES)
        return FILTER_ACCEL_ERR_UNSUPPORTED;

    filter_accel_prepare_input_range(page_addr,
                                     (page_count - 1U) * page_stride +
                                     FILTER_ACCEL_PAGE_BYTES);

    for (uint32_t i = 0; i < page_count; i++)
        count_table[i] = tuple_counts[i];
    filter_accel_flush_range(hit_addr,
                             FILTER_ACCEL_BATCH_COUNT_OFFSET +
                             page_count * sizeof(uint32_t));

    filter_accel_write_reg(FILTER_ACCEL_REG_CONTROL, FILTER_ACCEL_CONTROL_CLEAR);
    filter_accel_write_reg(FILTER_ACCEL_REG_PAGE_ADDR, page_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_TUPDESC_ADDR, tupdesc_addr, &cached_tupdesc_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_FILTER_ADDR, filter_addr, &cached_filter_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_QUAL_ADDR, qual_addr, &cached_qual_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_HIT_ADDR, hit_addr, &cached_hit_addr);
    filter_accel_write_reg(FILTER_ACCEL_REG_PAGE_ID, page_id);
    filter_accel_write_cached(FILTER_ACCEL_REG_NATTS, natts, &cached_natts);
    filter_accel_write_cached(FILTER_ACCEL_REG_FILTER_CNT, filter_cnt, &cached_filter_cnt);
    filter_accel_write_cached(FILTER_ACCEL_REG_PAGE_STRIDE, page_stride, &cached_page_stride);
    filter_accel_write_cached(FILTER_ACCEL_REG_HIT_STRIDE, hit_stride, &cached_hit_stride);
    filter_accel_write_cached(FILTER_ACCEL_REG_TUPLE_CNT_ADDR, hit_addr, &cached_tuple_cnt_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_PAGE_ADDR_TABLE_ADDR, 0U, &cached_page_addr_table_addr);
    filter_accel_program_projection();
    filter_accel_write_reg(FILTER_ACCEL_REG_PAGE_COUNT, page_count);
    filter_accel_write_reg(FILTER_ACCEL_REG_TUPLE_CNT, 0U);
    cache_valid = 1;
    filter_accel_write_reg(FILTER_ACCEL_REG_CONTROL, FILTER_ACCEL_CONTROL_START);

    for (polls = 0; polls < (FILTER_ACCEL_TIMEOUT_POLLS * page_count); polls++) {
        status = filter_accel_read_reg(FILTER_ACCEL_REG_STATUS);
        if (status & FILTER_ACCEL_STATUS_DONE) {
            if (status & FILTER_ACCEL_STATUS_UNSUPPORTED)
                return FILTER_ACCEL_ERR_UNSUPPORTED;
            if (status & FILTER_ACCEL_STATUS_ERROR)
                return FILTER_ACCEL_ERR_HW;
            Xil_DCacheInvalidateRange(hit_addr,
                                      count_only_enabled ?
                                      (page_count * sizeof(uint32_t)) :
                                      (FILTER_ACCEL_BATCH_DESC_OFFSET +
                                       page_count * hit_stride));
            filter_accel_invalidate_projection(hit_addr, page_count);
            filter_accel_invalidate_compute(hit_addr, page_count);
            return FILTER_ACCEL_OK;
        }
        filter_accel_poll_service();
    }

    return FILTER_ACCEL_ERR_TIMEOUT;
}

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
                              uint32_t hit_stride)
{
    uint32_t polls;
    int rc;

    rc = filter_accel_start_batch_sg(page_addrs, tupdesc_addr, filter_addr,
                                     qual_addr, hit_addr, page_id, natts,
                                     filter_cnt, tuple_counts, page_count,
                                     hit_stride);
    if (rc != FILTER_ACCEL_OK)
        return rc;

    for (polls = 0; polls < (FILTER_ACCEL_TIMEOUT_POLLS * page_count); polls++) {
        rc = filter_accel_poll_batch_sg(hit_addr, page_count, hit_stride);
        if (rc != FILTER_ACCEL_PENDING)
            return rc;
        filter_accel_poll_service();
    }

    return FILTER_ACCEL_ERR_TIMEOUT;
}

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
                                uint32_t hit_stride)
{
    volatile uint32_t *count_table =
        (volatile uint32_t *)(uintptr_t)hit_addr;
    volatile uint32_t *addr_table =
        (volatile uint32_t *)(uintptr_t)(hit_addr + FILTER_ACCEL_BATCH_ADDR_OFFSET);

    if (!page_addrs || !tuple_counts || page_count == 0U)
        return FILTER_ACCEL_ERR_HW;

    if (page_count > FILTER_ACCEL_MAX_BATCH_PAGES)
        return FILTER_ACCEL_ERR_UNSUPPORTED;

    if (page_count == 1U) {
        filter_accel_start_page_hw(page_addrs[0], tupdesc_addr, filter_addr,
                                   qual_addr,
                                   hit_addr + FILTER_ACCEL_BATCH_DESC_OFFSET,
                                   page_id, natts, filter_cnt, tuple_counts[0]);
        return FILTER_ACCEL_OK;
    }

    for (uint32_t i = 0; i < page_count; i++) {
        filter_accel_prepare_input_range(page_addrs[i], FILTER_ACCEL_PAGE_BYTES);
        count_table[i] = tuple_counts[i];
        addr_table[i] = page_addrs[i];
    }
    filter_accel_flush_range(hit_addr,
                             FILTER_ACCEL_BATCH_ADDR_OFFSET +
                             page_count * sizeof(uint32_t));

    filter_accel_write_reg(FILTER_ACCEL_REG_CONTROL, FILTER_ACCEL_CONTROL_CLEAR);
    filter_accel_write_reg(FILTER_ACCEL_REG_PAGE_ADDR, page_addrs[0]);
    filter_accel_write_cached(FILTER_ACCEL_REG_TUPDESC_ADDR, tupdesc_addr, &cached_tupdesc_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_FILTER_ADDR, filter_addr, &cached_filter_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_QUAL_ADDR, qual_addr, &cached_qual_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_HIT_ADDR, hit_addr, &cached_hit_addr);
    filter_accel_write_reg(FILTER_ACCEL_REG_PAGE_ID, page_id);
    filter_accel_write_cached(FILTER_ACCEL_REG_NATTS, natts, &cached_natts);
    filter_accel_write_cached(FILTER_ACCEL_REG_FILTER_CNT, filter_cnt, &cached_filter_cnt);
    filter_accel_write_cached(FILTER_ACCEL_REG_PAGE_STRIDE, 0U, &cached_page_stride);
    filter_accel_write_cached(FILTER_ACCEL_REG_HIT_STRIDE, hit_stride, &cached_hit_stride);
    filter_accel_write_cached(FILTER_ACCEL_REG_TUPLE_CNT_ADDR, hit_addr, &cached_tuple_cnt_addr);
    filter_accel_write_cached(FILTER_ACCEL_REG_PAGE_ADDR_TABLE_ADDR,
                              hit_addr + FILTER_ACCEL_BATCH_ADDR_OFFSET,
                              &cached_page_addr_table_addr);
    filter_accel_program_projection();
    filter_accel_write_reg(FILTER_ACCEL_REG_PAGE_COUNT, page_count);
    filter_accel_write_reg(FILTER_ACCEL_REG_TUPLE_CNT, 0U);
    cache_valid = 1;
    filter_accel_write_reg(FILTER_ACCEL_REG_CONTROL, FILTER_ACCEL_CONTROL_START);

    return FILTER_ACCEL_OK;
}

int filter_accel_poll_batch_sg(uint32_t hit_addr,
                               uint32_t page_count,
                               uint32_t hit_stride)
{
    uint32_t status;

    if (page_count == 0U || page_count > FILTER_ACCEL_MAX_BATCH_PAGES)
        return FILTER_ACCEL_ERR_HW;

    if (page_count == 1U) {
        uint32_t hit_count;
        int rc = filter_accel_poll_page_hw(
            hit_addr + FILTER_ACCEL_BATCH_DESC_OFFSET, &hit_count);

        if (rc == FILTER_ACCEL_OK)
            *(volatile uint32_t *)(uintptr_t)hit_addr = hit_count;
        return rc;
    }

    status = filter_accel_read_reg(FILTER_ACCEL_REG_STATUS);
    if (!(status & FILTER_ACCEL_STATUS_DONE))
        return FILTER_ACCEL_PENDING;
    if (status & FILTER_ACCEL_STATUS_UNSUPPORTED)
        return FILTER_ACCEL_ERR_UNSUPPORTED;
    if (status & FILTER_ACCEL_STATUS_ERROR)
        return FILTER_ACCEL_ERR_HW;

    Xil_DCacheInvalidateRange(hit_addr,
                              count_only_enabled ?
                              (page_count * sizeof(uint32_t)) :
                              (FILTER_ACCEL_BATCH_DESC_OFFSET +
                               page_count * hit_stride));
    filter_accel_invalidate_projection(hit_addr, page_count);
    filter_accel_invalidate_compute(hit_addr, page_count);
    return FILTER_ACCEL_OK;
}
