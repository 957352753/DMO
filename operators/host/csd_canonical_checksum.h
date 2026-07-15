#ifndef CSD_CANONICAL_CHECKSUM_H_
#define CSD_CANONICAL_CHECKSUM_H_

#include <stddef.h>
#include <stdint.h>

#define CSD_CANONICAL_COLUMN_FLAG_NULL 0x0001U

#define CSD_CANONICAL_OID_BYTEA   17U
#define CSD_CANONICAL_OID_INT8    20U
#define CSD_CANONICAL_OID_INT4    23U
#define CSD_CANONICAL_OID_TEXT    25U
#define CSD_CANONICAL_OID_FLOAT8  701U
#define CSD_CANONICAL_OID_BPCHAR  1042U
#define CSD_CANONICAL_OID_VARCHAR 1043U

uint64_t csd_canonical_checksum_begin(void);
void csd_canonical_checksum_row(uint64_t *checksum);
int csd_canonical_checksum_raw_field(uint64_t *checksum,
	uint16_t flags, const void *data, uint16_t length);
int csd_canonical_checksum_pg_binary_field(uint64_t *checksum,
	uint32_t is_null, unsigned int type_oid, const void *data, uint32_t length);

#endif /* CSD_CANONICAL_CHECKSUM_H_ */
