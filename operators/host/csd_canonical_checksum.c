#include "csd_canonical_checksum.h"

#define CSD_CANONICAL_FNV_OFFSET UINT64_C(1469598103934665603)
#define CSD_CANONICAL_FNV_PRIME UINT64_C(1099511628211)
#define CSD_CANONICAL_ROW_MARKER UINT32_C(0x726f7701)

static void checksum_bytes(uint64_t *checksum, const void *data, size_t bytes)
{
	const uint8_t *input = (const uint8_t *)data;
	for(size_t i = 0U; i < bytes; i++)
	{
		*checksum ^= input[i];
		*checksum *= CSD_CANONICAL_FNV_PRIME;
	}
}

static void checksum_u16_le(uint64_t *checksum, uint16_t value)
{
	uint8_t bytes[2] = {(uint8_t)value, (uint8_t)(value >> 8)};
	checksum_bytes(checksum, bytes, sizeof(bytes));
}

static void checksum_u32_le(uint64_t *checksum, uint32_t value)
{
	uint8_t bytes[4] = {
		(uint8_t)value, (uint8_t)(value >> 8),
		(uint8_t)(value >> 16), (uint8_t)(value >> 24)
	};
	checksum_bytes(checksum, bytes, sizeof(bytes));
}

uint64_t csd_canonical_checksum_begin(void)
{
	return CSD_CANONICAL_FNV_OFFSET;
}

void csd_canonical_checksum_row(uint64_t *checksum)
{
	if(checksum)
		checksum_u32_le(checksum, CSD_CANONICAL_ROW_MARKER);
}

int csd_canonical_checksum_raw_field(uint64_t *checksum,
									 uint16_t flags,
									 const void *data,
									 uint16_t length)
{
	if(!checksum || (flags & ~CSD_CANONICAL_COLUMN_FLAG_NULL) != 0U ||
	   ((flags & CSD_CANONICAL_COLUMN_FLAG_NULL) != 0U && (data || length != 0U)) ||
	   ((flags & CSD_CANONICAL_COLUMN_FLAG_NULL) == 0U && length != 0U && !data))
		return -1;
	checksum_u16_le(checksum, flags);
	checksum_u16_le(checksum, length);
	if(data && length != 0U)
		checksum_bytes(checksum, data, length);
	return 0;
}

int csd_canonical_checksum_pg_binary_field(uint64_t *checksum,
									   uint32_t is_null,
									   unsigned int type_oid,
									   const void *data,
									   uint32_t length)
{
	const uint8_t *bytes = (const uint8_t *)data;
	uint16_t flags;
	uint32_t reverse = 0U;

	if(!checksum || is_null > 1U || length > UINT16_MAX ||
	   (!is_null && length != 0U && !data))
		return -1;
	flags = is_null ? CSD_CANONICAL_COLUMN_FLAG_NULL : 0U;
	checksum_u16_le(checksum, flags);
	checksum_u16_le(checksum, (uint16_t)length);
	if(is_null)
		return length == 0U ? 0 : -1;

	if(type_oid == CSD_CANONICAL_OID_INT4)
		reverse = length == 4U;
	else if(type_oid == CSD_CANONICAL_OID_INT8 || type_oid == CSD_CANONICAL_OID_FLOAT8)
		reverse = length == 8U;
	else if(type_oid != CSD_CANONICAL_OID_TEXT && type_oid != CSD_CANONICAL_OID_VARCHAR &&
			type_oid != CSD_CANONICAL_OID_BPCHAR && type_oid != CSD_CANONICAL_OID_BYTEA)
		return -1;

	if((type_oid == CSD_CANONICAL_OID_INT4 || type_oid == CSD_CANONICAL_OID_INT8 ||
		type_oid == CSD_CANONICAL_OID_FLOAT8) && reverse == 0U)
		return -1;
	if(reverse)
	{
		for(uint32_t i = length; i > 0U; i--)
			checksum_bytes(checksum, &bytes[i - 1U], 1U);
	}
	else if(length != 0U)
		checksum_bytes(checksum, bytes, length);
	return 0;
}
