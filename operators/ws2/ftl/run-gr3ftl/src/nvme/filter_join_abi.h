#ifndef FILTER_JOIN_ABI_H_
#define FILTER_JOIN_ABI_H_

#include <stdint.h>

#include "filter_types_abi.h"

#define FILTER_JOIN_MAX_KEYS              4U
#define FILTER_JOIN_DESC_OFFSET           0x400U
#define FILTER_JOIN_DESC_MAGIC            0x314e4a48U /* "HJN1" */
#define FILTER_JOIN_DESC_VERSION          1U

#define FILTER_JOIN_MODE_DISABLED         0U
#define FILTER_JOIN_MODE_ARM              1U
#define FILTER_JOIN_MODE_HYBRID           2U
#define FILTER_JOIN_MODE_AUTO             3U

#define FILTER_JOIN_PHASE_BUILD           1U
#define FILTER_JOIN_PHASE_PROBE           2U

#define FILTER_JOIN_DESC_FLAG_RESET_ON_BUILD 0x01U
#define FILTER_JOIN_DESC_FLAG_COUNT_ONLY     0x02U

#define FILTER_JOIN_RESULT_HEADER_OFFSET  128U
#define FILTER_JOIN_RESULT_DATA_OFFSET    256U
#define FILTER_JOIN_RESULT_MAGIC          0x31524a48U /* "HJR1" */
#define FILTER_JOIN_RESULT_VERSION        1U

#define FILTER_JOIN_EFFECTIVE_NONE        0U
#define FILTER_JOIN_EFFECTIVE_ARM         1U
#define FILTER_JOIN_EFFECTIVE_HYBRID      2U
#define FILTER_JOIN_EFFECTIVE_MIXED       3U

#define FILTER_JOIN_RESULT_FLAG_BUILD_FULL     0x00000001U
#define FILTER_JOIN_RESULT_FLAG_OUTPUT_FULL    0x00000002U
#define FILTER_JOIN_RESULT_FLAG_BAD_TUPLE      0x00000004U
#define FILTER_JOIN_RESULT_FLAG_SESSION_MISS   0x00000008U
#define FILTER_JOIN_RESULT_FLAG_FPGA_FALLBACK  0x00000010U
#define FILTER_JOIN_RESULT_FLAG_INVALID_HASHES 0x00000020U

#define FILTER_JOIN_ERROR_NONE             0U
#define FILTER_JOIN_ERROR_BAD_DESCRIPTOR   1U
#define FILTER_JOIN_ERROR_BAD_TUPLE        2U
#define FILTER_JOIN_ERROR_WORKSPACE_FULL   3U
#define FILTER_JOIN_ERROR_RESULT_FULL      4U
#define FILTER_JOIN_ERROR_SESSION_MISMATCH 5U
#define FILTER_JOIN_ERROR_BAD_FPGA_HASHES  6U

#define FILTER_JOIN_FPGA_HASH_MAGIC         0x31485346U /* "FSH1" */
#define FILTER_JOIN_FPGA_HASH_VERSION       1U
#define FILTER_JOIN_FPGA_HASH_HEADER_BYTES  16U

typedef struct filter_join_desc
{
	uint32_t magic;
	uint16_t version;
	uint16_t struct_bytes;
	uint8_t mode;
	uint8_t phase;
	uint8_t key_count;
	uint8_t flags;
	uint32_t session_id;
	uint32_t bucket_count;
	uint32_t max_build_rows;
	uint32_t max_output_rows;
	uint32_t max_tuple_bytes;
	uint32_t hash_seed;
	uint16_t build_natts;
	uint16_t probe_natts;
	uint8_t build_key_attr[FILTER_JOIN_MAX_KEYS];
	uint8_t probe_key_attr[FILTER_JOIN_MAX_KEYS];
	uint8_t key_type[FILTER_JOIN_MAX_KEYS];
	uint8_t key_flags[FILTER_JOIN_MAX_KEYS];
	uint8_t reserved[72];
} filter_join_desc_t;

typedef struct filter_join_result_header
{
	uint32_t magic;
	uint16_t version;
	uint16_t header_bytes;
	uint8_t requested_mode;
	uint8_t effective_mode;
	uint8_t phase;
	uint8_t key_count;
	uint32_t flags;
	uint32_t session_id;
	uint32_t error_code;
	uint32_t bytes_used;
	uint32_t bucket_count;
	uint64_t build_rows;
	uint64_t probe_rows;
	uint64_t match_rows;
	uint64_t hash_collisions;
	uint32_t max_chain;
	uint32_t dropped_rows;
	uint64_t workspace_used;
	uint8_t build_key_attr[FILTER_JOIN_MAX_KEYS];
	uint8_t probe_key_attr[FILTER_JOIN_MAX_KEYS];
	uint8_t key_type[FILTER_JOIN_MAX_KEYS];
	uint8_t key_flags[FILTER_JOIN_MAX_KEYS];
	uint64_t output_rows;
	uint8_t reserved[24];
} filter_join_result_header_t;

typedef struct filter_join_row
{
	uint32_t row_bytes;
	uint32_t build_tuple_bytes;
	uint32_t probe_tuple_bytes;
	uint32_t hash;
	uint32_t flags;
	uint32_t reserved;
} filter_join_row_t;

typedef struct filter_join_fpga_hash_header
{
	uint32_t magic;
	uint16_t version;
	uint16_t header_bytes;
	uint32_t flags;
	uint32_t hash_count;
} filter_join_fpga_hash_header_t;

#endif /* FILTER_JOIN_ABI_H_ */
