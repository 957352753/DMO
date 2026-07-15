#ifndef FILTER_TUPLE_H_
#define FILTER_TUPLE_H_

#include <stdint.h>

#include "filter_common.h"

#define FILTER_TUPLE_VALUE_FLAG_NULL 0x0001U

#define FILTER_TUPLE_ERROR_BAD_TUPLE           1
#define FILTER_TUPLE_ERROR_UNSUPPORTED_VARLENA 2

typedef struct filter_tuple_value
{
	const unsigned char *data;
	uint16_t length;
	uint16_t flags;
} filter_tuple_value_t;

/* Call once when a new operator descriptor is installed. */
void filter_tuple_reset_decoder_cache(void);

/* Decode only attributes selected by wanted_mask. Attribute indexes are
 * zero-based. Values point into tuple and remain valid while tuple is valid. */
int filter_tuple_decode(const unsigned char *tuple,
						uint32_t tuple_len,
						const tupdesc_t *tupdesc,
						uint32_t natts,
						uint32_t wanted_mask,
						filter_tuple_value_t values[MAX_COL]);

#endif /* FILTER_TUPLE_H_ */
