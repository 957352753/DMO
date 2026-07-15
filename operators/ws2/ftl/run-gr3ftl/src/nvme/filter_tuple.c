#include "filter_tuple.h"

#include <limits.h>
#include <string.h>

#define HEAP_HASNULL 0x0001U

typedef struct filter_tuple_fixed_cache
{
	const tupdesc_t *tupdesc;
	uint32_t natts;
	uint32_t wanted_mask;
	uint32_t max_end;
	uint16_t offset[MAX_COL];
	uint16_t length[MAX_COL];
	uint8_t hoff;
	uint8_t valid;
} filter_tuple_fixed_cache_t;

static filter_tuple_fixed_cache_t fixed_cache;

void filter_tuple_reset_decoder_cache(void)
{
	memset(&fixed_cache, 0, sizeof(fixed_cache));
}

static uint16_t filter_tuple_read_u16(const unsigned char *ptr)
{
	uint16_t value;

	memcpy(&value, ptr, sizeof(value));
	return value;
}

static uint32_t filter_tuple_read_u32(const unsigned char *ptr)
{
	uint32_t value;

	memcpy(&value, ptr, sizeof(value));
	return value;
}

/* Returns 1 when fixed-width decoding completed, 0 when the generic decoder
 * is required, and a negative tuple error for a malformed cached layout. */
static int filter_tuple_decode_fixed(const unsigned char *tuple,
									 uint32_t tuple_len,
									 const tupdesc_t *tupdesc,
									 uint32_t natts,
									 uint32_t wanted_mask,
									 uint16_t infomask,
									 uint8_t hoff,
									 filter_tuple_value_t values[MAX_COL])
{
	uint32_t position;
	uint32_t highest = 0U;

	if((infomask & HEAP_HASNULL) != 0U)
		return 0;
	if(fixed_cache.valid && fixed_cache.tupdesc == tupdesc &&
	   fixed_cache.natts == natts && fixed_cache.wanted_mask == wanted_mask &&
	   fixed_cache.hoff == hoff)
	{
		if(fixed_cache.max_end > tuple_len)
			return -FILTER_TUPLE_ERROR_BAD_TUPLE;
		for(uint32_t attr = 0; attr < natts; attr++)
		{
			if((wanted_mask & (1U << attr)) == 0U)
				continue;
			values[attr].data = tuple + fixed_cache.offset[attr];
			values[attr].length = fixed_cache.length[attr];
			values[attr].flags = 0U;
		}
		return 1;
	}

	for(uint32_t attr = 0; attr < natts; attr++)
		if((wanted_mask & (1U << attr)) != 0U)
			highest = attr;

	position = hoff;
	for(uint32_t attr = 0; attr <= highest; attr++)
	{
		int32_t desc_len = tupdesc->desc_len_in[attr];
		uint32_t align_mask = (uint16_t)tupdesc->desc_align_in[attr];

		if(desc_len <= 0 || align_mask > 7U)
			return 0;
		position = (position + align_mask) & ~align_mask;
		if(position > tuple_len || (uint32_t)desc_len > tuple_len - position)
			return -FILTER_TUPLE_ERROR_BAD_TUPLE;
		if((wanted_mask & (1U << attr)) != 0U)
		{
			fixed_cache.offset[attr] = (uint16_t)position;
			fixed_cache.length[attr] = (uint16_t)desc_len;
		}
		position += (uint32_t)desc_len;
	}

	fixed_cache.tupdesc = tupdesc;
	fixed_cache.natts = natts;
	fixed_cache.wanted_mask = wanted_mask;
	fixed_cache.max_end = position;
	fixed_cache.hoff = hoff;
	fixed_cache.valid = 1U;
	for(uint32_t attr = 0; attr < natts; attr++)
	{
		if((wanted_mask & (1U << attr)) == 0U)
			continue;
		values[attr].data = tuple + fixed_cache.offset[attr];
		values[attr].length = fixed_cache.length[attr];
		values[attr].flags = 0U;
	}
	return 1;
}

int filter_tuple_decode(const unsigned char *tuple,
						uint32_t tuple_len,
						const tupdesc_t *tupdesc,
						uint32_t natts,
						uint32_t wanted_mask,
						filter_tuple_value_t values[MAX_COL])
{
	uint16_t infomask;
	uint32_t position;
	uint32_t has_null;
	uint32_t remaining_mask = wanted_mask;
	int fixed_rc;

	if(!tuple || !tupdesc || !values || natts == 0U || natts > MAX_COL ||
	   tuple_len < 23U || (wanted_mask >> natts) != 0U)
		return -FILTER_TUPLE_ERROR_BAD_TUPLE;

	infomask = filter_tuple_read_u16(tuple + 20U);
	position = tuple[22U];
	has_null = (infomask & HEAP_HASNULL) != 0U;
	if(position < 23U || position > tuple_len)
		return -FILTER_TUPLE_ERROR_BAD_TUPLE;
	if(has_null && (23U + ((natts + 7U) >> 3)) > position)
		return -FILTER_TUPLE_ERROR_BAD_TUPLE;
	fixed_rc = filter_tuple_decode_fixed(tuple, tuple_len, tupdesc, natts,
		wanted_mask, infomask, (uint8_t)position, values);
	if(fixed_rc != 0)
		return fixed_rc > 0 ? 0 : fixed_rc;
	memset(values, 0, sizeof(*values) * MAX_COL);

	for(uint32_t attr = 0; attr < natts; attr++)
	{
		uint32_t is_null = has_null &&
			((tuple[23U + (attr >> 3)] & (1U << (attr & 7U))) == 0U);
		int32_t desc_len = tupdesc->desc_len_in[attr];
		uint32_t align_mask = (uint16_t)tupdesc->desc_align_in[attr];
		const unsigned char *data;
		uint32_t data_len;

		if(is_null)
		{
			if((wanted_mask & (1U << attr)) != 0U)
			{
				values[attr].flags = FILTER_TUPLE_VALUE_FLAG_NULL;
				remaining_mask &= ~(1U << attr);
			}
			if(remaining_mask == 0U)
				return 0;
			continue;
		}

		if(align_mask > 7U)
			return -FILTER_TUPLE_ERROR_BAD_TUPLE;

		if(desc_len > 0)
		{
			position = (position + align_mask) & ~align_mask;
			if(position > tuple_len || (uint32_t)desc_len > tuple_len - position)
				return -FILTER_TUPLE_ERROR_BAD_TUPLE;
			data = tuple + position;
			data_len = (uint32_t)desc_len;
			position += data_len;
		}
		else
		{
			uint32_t total_len;
			uint32_t header_len;

			if(position >= tuple_len)
				return -FILTER_TUPLE_ERROR_BAD_TUPLE;
			if(tuple[position] == 0U)
				position = (position + align_mask) & ~align_mask;
			if(position >= tuple_len)
				return -FILTER_TUPLE_ERROR_BAD_TUPLE;

			if((tuple[position] & 0x01U) != 0U)
			{
				total_len = tuple[position] >> 1;
				header_len = 1U;
				if(total_len < header_len)
					return -FILTER_TUPLE_ERROR_UNSUPPORTED_VARLENA;
			}
			else
			{
				uint32_t header;

				if(tuple_len - position < 4U)
					return -FILTER_TUPLE_ERROR_BAD_TUPLE;
				header = filter_tuple_read_u32(tuple + position);
				if((header & 0x03U) != 0U)
					return -FILTER_TUPLE_ERROR_UNSUPPORTED_VARLENA;
				total_len = header >> 2;
				header_len = 4U;
			}

			if(total_len < header_len || total_len > tuple_len - position)
				return -FILTER_TUPLE_ERROR_BAD_TUPLE;
			data = tuple + position + header_len;
			data_len = total_len - header_len;
			position += total_len;
		}

		if(data_len > UINT16_MAX)
			return -FILTER_TUPLE_ERROR_BAD_TUPLE;
		if((wanted_mask & (1U << attr)) != 0U)
		{
			values[attr].data = data;
			values[attr].length = (uint16_t)data_len;
			remaining_mask &= ~(1U << attr);
			if(remaining_mask == 0U)
				return 0;
		}
	}

	return 0;
}
