#ifndef FILTER_COMPUTE_ABI_H_
#define FILTER_COMPUTE_ABI_H_

#include <stdint.h>

#include "filter_types_abi.h"

#define FILTER_COMPUTE_MAX_OPS              8U
#define FILTER_COMPUTE_DESC_OFFSET          0x300U
#define FILTER_COMPUTE_DESC_MAGIC           0x31504d43U /* "CMP1" */
#define FILTER_COMPUTE_DESC_VERSION         1U

#define FILTER_COMPUTE_MODE_DISABLED        0U
#define FILTER_COMPUTE_MODE_ARM             1U
#define FILTER_COMPUTE_MODE_FPGA            2U
#define FILTER_COMPUTE_MODE_AUTO            3U

#define FILTER_COMPUTE_KIND_AGGREGATE       1U
#define FILTER_COMPUTE_KIND_SORT            2U

#define FILTER_COMPUTE_OP_SUM               1U
#define FILTER_COMPUTE_OP_MIN               2U
#define FILTER_COMPUTE_OP_MAX               3U
#define FILTER_COMPUTE_OP_SORT              4U

#define FILTER_COMPUTE_OP_FLAG_DESC         0x01U
#define FILTER_COMPUTE_OP_FLAG_NULLS_FIRST  0x02U

#define FILTER_COMPUTE_RESULT_HEADER_OFFSET 128U
#define FILTER_COMPUTE_RESULT_DATA_OFFSET   256U
#define FILTER_COMPUTE_RESULT_MAGIC         0x31524343U /* "CCR1" */
#define FILTER_COMPUTE_RESULT_VERSION       1U

#define FILTER_COMPUTE_EFFECTIVE_NONE       0U
#define FILTER_COMPUTE_EFFECTIVE_ARM        1U
#define FILTER_COMPUTE_EFFECTIVE_FPGA       2U
#define FILTER_COMPUTE_EFFECTIVE_MIXED      3U

#define FILTER_COMPUTE_RESULT_FLAG_FULL          0x00000001U
#define FILTER_COMPUTE_RESULT_FLAG_FPGA_FALLBACK 0x00000002U
#define FILTER_COMPUTE_RESULT_FLAG_BAD_TUPLE     0x00000004U
#define FILTER_COMPUTE_RESULT_FLAG_OVERFLOW      0x00000008U
#define FILTER_COMPUTE_RESULT_FLAG_TRUNCATED     0x00000010U
#define FILTER_COMPUTE_RESULT_FLAG_INVALID_STAGE 0x00000020U
#define FILTER_COMPUTE_RESULT_FLAG_NAN            0x00000040U

#define FILTER_COMPUTE_VALUE_FLAG_NULL       0x01U
#define FILTER_COMPUTE_VALUE_FLAG_OVERFLOW   0x02U
#define FILTER_COMPUTE_VALUE_FLAG_NAN        0x04U

#define FILTER_COMPUTE_SORT_ROW_FLAG_KEY_NULL 0x0001U

#define FILTER_COMPUTE_ERROR_NONE              0U
#define FILTER_COMPUTE_ERROR_BAD_DESCRIPTOR    1U
#define FILTER_COMPUTE_ERROR_BAD_TUPLE         2U
#define FILTER_COMPUTE_ERROR_RESULT_FULL       3U
#define FILTER_COMPUTE_ERROR_WORKSPACE_FULL    4U
#define FILTER_COMPUTE_ERROR_UNSUPPORTED_TYPE  5U
#define FILTER_COMPUTE_ERROR_BAD_FPGA_PARTIAL  6U

#define FILTER_COMPUTE_FPGA_PARTIAL_MAGIC      0x31414746U /* "FGA1" */
#define FILTER_COMPUTE_FPGA_PARTIAL_VERSION    1U
#define FILTER_COMPUTE_FPGA_PARTIAL_BYTES      64U

#define FILTER_COMPUTE_FPGA_OP_SUM             0x01U
#define FILTER_COMPUTE_FPGA_OP_MIN             0x02U
#define FILTER_COMPUTE_FPGA_OP_MAX             0x04U

typedef struct filter_compute_op_desc
{
	uint8_t opcode;
	uint8_t value_type;
	uint8_t attr_index;
	uint8_t flags;
	uint32_t reserved;
} filter_compute_op_desc_t;

typedef struct filter_compute_desc
{
	uint32_t magic;
	uint16_t version;
	uint16_t struct_bytes;
	uint8_t mode;
	uint8_t kind;
	uint8_t op_count;
	uint8_t flags;
	uint32_t sort_limit;
	uint32_t max_tuple_bytes;
	filter_compute_op_desc_t ops[FILTER_COMPUTE_MAX_OPS];
	uint8_t reserved[44];
} filter_compute_desc_t;

typedef struct filter_compute_result_header
{
	uint32_t magic;
	uint16_t version;
	uint16_t header_bytes;
	uint8_t requested_mode;
	uint8_t effective_mode;
	uint8_t kind;
	uint8_t op_count;
	uint32_t flags;
	uint64_t input_rows;
	uint64_t output_rows;
	uint64_t arm_rows;
	uint64_t fpga_rows;
	uint32_t bytes_used;
	uint32_t error_code;
	uint32_t sort_limit;
	uint32_t max_tuple_bytes;
	filter_compute_op_desc_t ops[FILTER_COMPUTE_MAX_OPS];
} filter_compute_result_header_t;

typedef struct filter_compute_aggregate_value
{
	uint8_t opcode;
	uint8_t value_type;
	uint8_t attr_index;
	uint8_t flags;
	uint32_t value_bytes;
	uint64_t nonnull_count;
	uint8_t value[16];
	uint8_t reserved[8];
} filter_compute_aggregate_value_t;

typedef struct filter_compute_sort_row
{
	uint32_t row_bytes;
	uint32_t tuple_bytes;
	uint32_t key_offset;
	uint16_t key_bytes;
	uint16_t flags;
	uint64_t ordinal;
} filter_compute_sort_row_t;

/* Fixed 64-byte per-page partial written by the FPGA. All 64-bit values are
 * represented as little-endian low/high words to keep the wire layout packed. */
typedef struct filter_compute_fpga_partial
{
	uint32_t magic;
	uint16_t version;
	uint16_t struct_bytes;
	uint32_t flags;
	uint8_t value_type;
	uint8_t attr_index;
	uint8_t op_mask;
	uint8_t reserved0;
	uint32_t nonnull_lo;
	uint32_t nonnull_hi;
	uint32_t null_lo;
	uint32_t null_hi;
	uint32_t sum_lo;
	uint32_t sum_hi;
	uint32_t min_lo;
	uint32_t min_hi;
	uint32_t max_lo;
	uint32_t max_hi;
	uint32_t reserved[2];
} filter_compute_fpga_partial_t;

#endif /* FILTER_COMPUTE_ABI_H_ */
