#ifndef CSD_OPERATOR_IO_H_
#define CSD_OPERATOR_IO_H_

#include <stddef.h>
#include <stdint.h>

#define CSD_OPERATOR_METADATA_BYTES (4U * 4096U)
#define CSD_OPERATOR_PAGE_BYTES 8192U
#define CSD_OPERATOR_LBA_BYTES 4096U
#define CSD_OPERATOR_MAX_COLUMNS 16U
#define CSD_OPERATOR_MAX_FILES 256U

typedef struct csd_operator_tuple_desc
{
	int16_t length[CSD_OPERATOR_MAX_COLUMNS];
	int16_t align_mask[CSD_OPERATOR_MAX_COLUMNS];
} csd_operator_tuple_desc_t;

typedef struct csd_operator_qual
{
	int32_t natts;
	int32_t filter_count;
	int32_t file_count;
	char filepath[256];
} csd_operator_qual_t;

typedef struct csd_operator_extent
{
	uint64_t start_lba;
	uint64_t lba_count;
} csd_operator_extent_t;

typedef struct csd_operator_extent_list
{
	csd_operator_extent_t *items;
	size_t count;
	size_t capacity;
	uint64_t total_lbas;
} csd_operator_extent_list_t;

int csd_operator_parse_tuple_desc(const char *spec,
								  csd_operator_tuple_desc_t *desc,
								  uint32_t *natts);

int csd_operator_collect_extents(const char *device_path,
							 const char *const *file_paths,
							 size_t file_count,
							 uint32_t max_lbas_per_request,
							 csd_operator_extent_list_t *out,
							 char *error,
							 size_t error_bytes);

void csd_operator_extent_list_free(csd_operator_extent_list_t *list);

void csd_operator_prepare_metadata(uint8_t metadata[CSD_OPERATOR_METADATA_BYTES],
							   const csd_operator_tuple_desc_t *desc,
							   uint32_t natts,
							   const csd_operator_extent_t *extent);

int csd_operator_open_device(const char *device_path, char *error, size_t error_bytes);

int csd_operator_execute(int device_fd,
					 const uint8_t metadata[CSD_OPERATOR_METADATA_BYTES],
					 uint8_t *result,
					 size_t result_bytes,
					 uint32_t timeout_ms,
					 char *error,
					 size_t error_bytes);

double csd_operator_monotonic_ms(void);

#endif /* CSD_OPERATOR_IO_H_ */
