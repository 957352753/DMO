#ifndef FILTER_PROJECTION_ABI_H_
#define FILTER_PROJECTION_ABI_H_

#include <stdint.h>

#define FILTER_PROJECTION_MAX_COLUMNS          16U

/* The descriptor lives in metadata page 2, after the legacy qual_t area. */
#define FILTER_PROJECTION_DESC_OFFSET          0x200U
#define FILTER_PROJECTION_DESC_MAGIC           0x314a5250U /* "PRJ1" */
#define FILTER_PROJECTION_DESC_VERSION         1U

#define FILTER_PROJECTION_MODE_DISABLED        0U
#define FILTER_PROJECTION_MODE_ARM             1U
#define FILTER_PROJECTION_MODE_FPGA            2U

#define FILTER_PROJECTION_FORMAT_COMPACT_V1    1U

#define FILTER_PROJECTION_RESULT_HEADER_OFFSET 128U
#define FILTER_PROJECTION_RESULT_DATA_OFFSET   256U
#define FILTER_PROJECTION_RESULT_MAGIC         0x31524a50U /* "PJR1" */
#define FILTER_PROJECTION_RESULT_VERSION       1U

#define FILTER_PROJECTION_EFFECTIVE_NONE       0U
#define FILTER_PROJECTION_EFFECTIVE_ARM        1U
#define FILTER_PROJECTION_EFFECTIVE_FPGA       2U
#define FILTER_PROJECTION_EFFECTIVE_MIXED      3U

#define FILTER_PROJECTION_RESULT_FLAG_FULL          0x00000001U
#define FILTER_PROJECTION_RESULT_FLAG_FPGA_FALLBACK 0x00000002U
#define FILTER_PROJECTION_RESULT_FLAG_INVALID_STAGE 0x00000004U
#define FILTER_PROJECTION_RESULT_FLAG_ARM_ERROR     0x00000008U

#define FILTER_PROJECTION_COLUMN_FLAG_NULL     0x0001U

#define FILTER_PROJECTION_ERROR_NONE                0U
#define FILTER_PROJECTION_ERROR_BAD_DESCRIPTOR      1U
#define FILTER_PROJECTION_ERROR_RESULT_FULL         2U
#define FILTER_PROJECTION_ERROR_BAD_TUPLE           3U
#define FILTER_PROJECTION_ERROR_UNSUPPORTED_VARLENA 4U
#define FILTER_PROJECTION_ERROR_BAD_FPGA_STAGE      5U

typedef struct filter_projection_desc
{
	uint32_t magic;
	uint16_t version;
	uint16_t struct_bytes;
	uint8_t mode;
	uint8_t output_format;
	uint8_t column_count;
	uint8_t flags;
	uint32_t max_row_bytes;
	uint8_t attr_index[FILTER_PROJECTION_MAX_COLUMNS];
	uint8_t reserved[32];
} filter_projection_desc_t;

typedef struct filter_projection_result_header
{
	uint32_t magic;
	uint16_t version;
	uint16_t header_bytes;
	uint8_t requested_mode;
	uint8_t effective_mode;
	uint8_t output_format;
	uint8_t column_count;
	uint32_t flags;
	uint32_t row_count;
	uint32_t bytes_used;
	uint32_t arm_rows;
	uint32_t fpga_rows;
	uint32_t dropped_rows;
	uint32_t error_code;
	uint32_t max_row_bytes;
	uint8_t attr_index[FILTER_PROJECTION_MAX_COLUMNS];
	uint8_t reserved[68];
} filter_projection_result_header_t;

typedef struct filter_projection_row_header
{
	uint32_t row_bytes;
	uint16_t column_count;
	uint16_t flags;
} filter_projection_row_header_t;

typedef struct filter_projection_column
{
	uint32_t data_offset;
	uint16_t data_length;
	uint16_t flags;
} filter_projection_column_t;

#endif /* FILTER_PROJECTION_ABI_H_ */
