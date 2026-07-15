#include "filter_join.h"

#include <stddef.h>
#include <string.h>

#include "filter_tuple.h"
#include "page.h"

#define FILTER_JOIN_ALIGNMENT 4U
#define FILTER_JOIN_DEFAULT_HASH_SEED 2166136261U

typedef struct filter_join_entry
{
	uint32_t hash;
	uint32_t next;
	uint32_t tuple_offset;
	uint32_t tuple_bytes;
	uint64_t inline_key;
} filter_join_entry_t;

typedef struct filter_join_session
{
	unsigned char *workspace;
	uint32_t workspace_bytes;
	uint32_t valid;
	uint32_t session_id;
	uint32_t bucket_count;
	uint32_t max_build_rows;
	uint32_t max_tuple_bytes;
	uint32_t hash_seed;
	uint32_t count_only;
	uint32_t inline_only;
	uint32_t build_natts;
	uint32_t key_count;
	uint8_t build_key_attr[FILTER_JOIN_MAX_KEYS];
	uint8_t key_type[FILTER_JOIN_MAX_KEYS];
	tupdesc_t build_tupdesc;
	uint32_t tupdesc_valid;
	uint32_t *buckets;
	filter_join_entry_t *entries;
	uint32_t payload_offset;
	uint32_t payload_used;
	uint32_t entry_count;
	uint64_t skipped_build_rows;
} filter_join_session_t;

typedef struct filter_join_request
{
	filter_join_desc_t config;
	unsigned char *result;
	uint32_t result_bytes;
	filter_join_result_header_t *header;
	uint32_t write_offset;
	uint32_t wanted_mask;
	uint32_t fpga_config;
	uint8_t fpga_compatible;
	uint8_t finalized;
} filter_join_request_t;

static filter_join_session_t join_session;
static filter_join_request_t join_request;

typedef char join_desc_size_must_be_128[
	(sizeof(filter_join_desc_t) == 128U) ? 1 : -1];
typedef char join_result_header_size_must_be_128[
	(sizeof(filter_join_result_header_t) == 128U) ? 1 : -1];
typedef char join_row_size_must_be_24[
	(sizeof(filter_join_row_t) == 24U) ? 1 : -1];
typedef char join_fpga_hash_header_size_must_be_16[
	(sizeof(filter_join_fpga_hash_header_t) == FILTER_JOIN_FPGA_HASH_HEADER_BYTES) ? 1 : -1];

static uint32_t join_align4(uint32_t value)
{
	return (value + FILTER_JOIN_ALIGNMENT - 1U) &
		   ~(FILTER_JOIN_ALIGNMENT - 1U);
}

static uint32_t join_power_of_two(uint32_t value)
{
	return value != 0U && (value & (value - 1U)) == 0U;
}

static uint32_t join_is_nan_bits(uint64_t bits)
{
	return ((bits >> 52) & 0x7ffU) == 0x7ffU &&
		   (bits & UINT64_C(0x000fffffffffffff)) != 0U;
}

static uint64_t join_canonical_float(uint64_t bits)
{
	if((bits & UINT64_C(0x7fffffffffffffff)) == 0U)
		return 0U;
	if(join_is_nan_bits(bits))
		return UINT64_C(0x7ff8000000000000);
	return bits;
}

static uint32_t join_hash_bytes(uint32_t hash,
							const unsigned char *data,
							uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		hash ^= data[i];
		hash *= 16777619U;
	}
	return hash;
}

static uint32_t join_value_type_valid(uint32_t type)
{
	return type == FILTER_VALUE_TYPE_INT32 ||
		   type == FILTER_VALUE_TYPE_INT64 ||
		   type == FILTER_VALUE_TYPE_FLOAT64 ||
		   type == FILTER_VALUE_TYPE_BYTES;
}

static uint32_t join_value_length_valid(uint32_t type, uint32_t length)
{
	if(type == FILTER_VALUE_TYPE_INT32)
		return length == 4U;
	if(type == FILTER_VALUE_TYPE_INT64 || type == FILTER_VALUE_TYPE_FLOAT64)
		return length == 8U;
	return type == FILTER_VALUE_TYPE_BYTES;
}

static void join_set_error(uint32_t error, uint32_t flags)
{
	if(!join_request.header)
		return;
	join_request.header->flags |= flags;
	if(join_request.header->error_code == FILTER_JOIN_ERROR_NONE)
		join_request.header->error_code = error;
}

void filter_join_set_workspace(void *workspace, uint32_t workspace_bytes)
{
	if(join_session.workspace != (unsigned char *)workspace ||
	   join_session.workspace_bytes != workspace_bytes)
	{
		memset(&join_session, 0, sizeof(join_session));
		join_session.workspace = (unsigned char *)workspace;
		join_session.workspace_bytes = workspace_bytes;
	}
}

void filter_join_disable(void)
{
	memset(&join_request, 0, sizeof(join_request));
}

void filter_join_reset_session(void)
{
	unsigned char *workspace = join_session.workspace;
	uint32_t workspace_bytes = join_session.workspace_bytes;

	memset(&join_session, 0, sizeof(join_session));
	join_session.workspace = workspace;
	join_session.workspace_bytes = workspace_bytes;
}

static uint32_t join_session_matches(const filter_join_desc_t *config)
{
	return join_session.valid &&
		join_session.session_id == config->session_id &&
		join_session.bucket_count == config->bucket_count &&
		join_session.max_build_rows == config->max_build_rows &&
		join_session.max_tuple_bytes == config->max_tuple_bytes &&
		join_session.hash_seed == config->hash_seed &&
		join_session.count_only ==
			((config->flags & FILTER_JOIN_DESC_FLAG_COUNT_ONLY) != 0U) &&
		join_session.build_natts == config->build_natts &&
		join_session.key_count == config->key_count &&
		memcmp(join_session.build_key_attr, config->build_key_attr,
			   FILTER_JOIN_MAX_KEYS) == 0 &&
		memcmp(join_session.key_type, config->key_type,
			   FILTER_JOIN_MAX_KEYS) == 0;
}

static int join_start_session(const filter_join_desc_t *config)
{
	uint64_t buckets_bytes = (uint64_t)config->bucket_count * sizeof(uint32_t);
	uint64_t entries_offset = join_align4((uint32_t)buckets_bytes);
	uint64_t entries_bytes = (uint64_t)config->max_build_rows * sizeof(filter_join_entry_t);
	uint64_t payload_offset = entries_offset + entries_bytes;
	uint32_t count_only = (config->flags & FILTER_JOIN_DESC_FLAG_COUNT_ONLY) != 0U;
	uint32_t inline_only = count_only && config->key_count == 1U &&
		config->key_type[0] != FILTER_VALUE_TYPE_BYTES;

	if(!join_session.workspace || buckets_bytes > UINT32_MAX ||
	   entries_bytes > UINT32_MAX || payload_offset > join_session.workspace_bytes ||
	   (!inline_only && join_session.workspace_bytes - payload_offset < 23U))
		return -1;

	memset(join_session.workspace, 0, (size_t)buckets_bytes);
	join_session.valid = 1U;
	join_session.session_id = config->session_id;
	join_session.bucket_count = config->bucket_count;
	join_session.max_build_rows = config->max_build_rows;
	join_session.max_tuple_bytes = config->max_tuple_bytes;
	join_session.hash_seed = config->hash_seed;
	join_session.count_only = count_only;
	join_session.inline_only = inline_only;
	join_session.build_natts = config->build_natts;
	join_session.key_count = config->key_count;
	memcpy(join_session.build_key_attr, config->build_key_attr,
		   FILTER_JOIN_MAX_KEYS);
	memcpy(join_session.key_type, config->key_type, FILTER_JOIN_MAX_KEYS);
	join_session.buckets = (uint32_t *)join_session.workspace;
	join_session.entries = (filter_join_entry_t *)
		(join_session.workspace + (uint32_t)entries_offset);
	join_session.payload_offset = (uint32_t)payload_offset;
	join_session.payload_used = 0U;
	join_session.entry_count = 0U;
	join_session.skipped_build_rows = 0U;
	join_session.tupdesc_valid = 0U;
	return 0;
}

static void join_select_fpga_path(uint32_t current_attr)
{
	uint32_t type = join_request.config.key_type[0];

	join_request.fpga_compatible = 0U;
	join_request.fpga_config = 0U;
	if(join_request.config.mode == FILTER_JOIN_MODE_ARM ||
	   join_request.config.key_count != 1U ||
	   (type != FILTER_VALUE_TYPE_INT32 && type != FILTER_VALUE_TYPE_INT64))
		return;
	join_request.fpga_compatible = 1U;
	/* [3:0] kind=2 (hash), [7:4] type, [15:8] key attribute. */
	join_request.fpga_config = 2U | (type << 4) | (current_attr << 8);
}

int filter_join_configure(const filter_join_desc_t *desc,
						  uint32_t natts,
						  uint32_t count_only)
{
	filter_join_desc_t config;
	const uint8_t *current_attrs;
	uint32_t declared_natts;

	filter_tuple_reset_decoder_cache();
	filter_join_disable();
	if(!desc || desc->magic != FILTER_JOIN_DESC_MAGIC)
		return 0;
	memcpy(&config, desc, sizeof(config));
	if(config.hash_seed == 0U)
		config.hash_seed = FILTER_JOIN_DEFAULT_HASH_SEED;
	if(config.version != FILTER_JOIN_DESC_VERSION ||
	   config.struct_bytes < sizeof(config) || count_only ||
	   natts == 0U || natts > MAX_COL ||
	   (config.mode != FILTER_JOIN_MODE_ARM &&
		config.mode != FILTER_JOIN_MODE_HYBRID &&
		config.mode != FILTER_JOIN_MODE_AUTO) ||
	   (config.phase != FILTER_JOIN_PHASE_BUILD &&
		config.phase != FILTER_JOIN_PHASE_PROBE) ||
	   config.key_count == 0U || config.key_count > FILTER_JOIN_MAX_KEYS ||
	   !join_power_of_two(config.bucket_count) ||
	   config.bucket_count < FILTER_JOIN_MIN_BUCKETS ||
	   config.bucket_count > FILTER_JOIN_MAX_BUCKETS ||
	   config.max_build_rows == 0U ||
	   config.max_build_rows > FILTER_JOIN_MAX_BUILD_ROWS ||
	   config.max_output_rows > FILTER_JOIN_MAX_OUTPUT_ROWS ||
	   (config.max_output_rows == 0U &&
		(config.flags & FILTER_JOIN_DESC_FLAG_COUNT_ONLY) == 0U) ||
	   config.max_tuple_bytes < 23U ||
	   config.max_tuple_bytes > FILTER_COMPUTE_MAX_TUPLE_BYTES ||
	   (config.flags & ~(FILTER_JOIN_DESC_FLAG_RESET_ON_BUILD |
						 FILTER_JOIN_DESC_FLAG_COUNT_ONLY)) != 0U)
		return -1;

	current_attrs = config.phase == FILTER_JOIN_PHASE_BUILD ?
		config.build_key_attr : config.probe_key_attr;
	declared_natts = config.phase == FILTER_JOIN_PHASE_BUILD ?
		config.build_natts : config.probe_natts;
	if(declared_natts != natts || config.build_natts == 0U ||
	   config.build_natts > MAX_COL || config.probe_natts == 0U ||
	   config.probe_natts > MAX_COL)
		return -1;
	for(uint32_t i = 0; i < config.key_count; i++)
	{
		if(!join_value_type_valid(config.key_type[i]) ||
		   config.build_key_attr[i] >= config.build_natts ||
		   config.probe_key_attr[i] >= config.probe_natts ||
		   current_attrs[i] >= natts || config.key_flags[i] != 0U)
			return -1;
	}

	if(config.phase == FILTER_JOIN_PHASE_BUILD)
	{
		if((config.flags & FILTER_JOIN_DESC_FLAG_RESET_ON_BUILD) != 0U ||
		   !join_session_matches(&config))
		{
			if(join_start_session(&config) < 0)
				return -1;
		}
	}
	else if(!join_session_matches(&config) || !join_session.tupdesc_valid)
	{
		return -1;
	}

	join_request.config = config;
	for(uint32_t i = 0; i < config.key_count; i++)
		join_request.wanted_mask |= 1U << current_attrs[i];
	join_select_fpga_path(current_attrs[0]);
	return 1;
}

void filter_join_reset_result(void *result_buffer, uint32_t result_bytes)
{
	filter_join_result_header_t *header;
	uint32_t page_count = 1U;

	if(!filter_join_enabled() || !result_buffer ||
	   result_bytes < FILTER_JOIN_RESULT_DATA_OFFSET)
		return;
	memset(result_buffer, 0, result_bytes);
	join_request.result = (unsigned char *)result_buffer;
	join_request.result_bytes = result_bytes;
	join_request.write_offset = FILTER_JOIN_RESULT_DATA_OFFSET;
	join_request.finalized = 0U;
	memcpy(join_request.result, &page_count, sizeof(page_count));
	header = (filter_join_result_header_t *)
		(join_request.result + FILTER_JOIN_RESULT_HEADER_OFFSET);
	join_request.header = header;
	header->magic = FILTER_JOIN_RESULT_MAGIC;
	header->version = FILTER_JOIN_RESULT_VERSION;
	header->header_bytes = sizeof(*header);
	header->requested_mode = join_request.config.mode;
	header->phase = join_request.config.phase;
	header->key_count = join_request.config.key_count;
	header->session_id = join_request.config.session_id;
	header->bytes_used = FILTER_JOIN_RESULT_DATA_OFFSET;
	header->bucket_count = join_request.config.bucket_count;
	header->build_rows = join_session.entry_count;
	memcpy(header->build_key_attr, join_request.config.build_key_attr,
		   FILTER_JOIN_MAX_KEYS);
	memcpy(header->probe_key_attr, join_request.config.probe_key_attr,
		   FILTER_JOIN_MAX_KEYS);
	memcpy(header->key_type, join_request.config.key_type,
		   FILTER_JOIN_MAX_KEYS);
	memcpy(header->key_flags, join_request.config.key_flags,
		   FILTER_JOIN_MAX_KEYS);
	if(join_request.config.phase == FILTER_JOIN_PHASE_BUILD)
		header->effective_mode = FILTER_JOIN_EFFECTIVE_ARM;
}

uint32_t filter_join_enabled(void)
{
	return join_request.config.mode != FILTER_JOIN_MODE_DISABLED;
}

uint32_t filter_join_phase(void)
{
	return join_request.config.phase;
}

uint32_t filter_join_fpga_enabled(void)
{
	return join_request.fpga_compatible;
}

uint32_t filter_join_fpga_config(void)
{
	return join_request.fpga_compatible ? join_request.fpga_config : 0U;
}

uint32_t filter_join_hash_seed(void)
{
	return join_request.config.hash_seed;
}

static int join_hash_values(const filter_tuple_value_t values[MAX_COL],
							const uint8_t attrs[FILTER_JOIN_MAX_KEYS],
							uint32_t *hash_out)
{
	uint32_t hash = join_request.config.hash_seed;

	for(uint32_t key = 0; key < join_request.config.key_count; key++)
	{
		const filter_tuple_value_t *value = &values[attrs[key]];
		uint32_t type = join_request.config.key_type[key];
		uint64_t bits;

		if((value->flags & FILTER_TUPLE_VALUE_FLAG_NULL) != 0U)
			return 0;
		if(!join_value_length_valid(type, value->length))
			return -1;
		hash = join_hash_bytes(hash, (const unsigned char *)&type, 1U);
		if(type == FILTER_VALUE_TYPE_FLOAT64)
		{
			memcpy(&bits, value->data, sizeof(bits));
			bits = join_canonical_float(bits);
			hash = join_hash_bytes(hash, (const unsigned char *)&bits, sizeof(bits));
		}
		else
			hash = join_hash_bytes(hash, value->data, value->length);
	}
	*hash_out = hash;
	return 1;
}

static int join_values_equal(const filter_tuple_value_t probe_values[MAX_COL],
							 const filter_tuple_value_t build_values[MAX_COL])
{
	for(uint32_t key = 0; key < join_request.config.key_count; key++)
	{
		const filter_tuple_value_t *probe =
			&probe_values[join_request.config.probe_key_attr[key]];
		const filter_tuple_value_t *build =
			&build_values[join_request.config.build_key_attr[key]];
		uint32_t type = join_request.config.key_type[key];

		if((probe->flags & FILTER_TUPLE_VALUE_FLAG_NULL) != 0U ||
		   (build->flags & FILTER_TUPLE_VALUE_FLAG_NULL) != 0U ||
		   !join_value_length_valid(type, probe->length) ||
		   !join_value_length_valid(type, build->length))
			return 0;
		if(type == FILTER_VALUE_TYPE_FLOAT64)
		{
			uint64_t left;
			uint64_t right;
			memcpy(&left, probe->data, sizeof(left));
			memcpy(&right, build->data, sizeof(right));
			if(join_canonical_float(left) != join_canonical_float(right))
				return 0;
		}
		else if(probe->length != build->length ||
				memcmp(probe->data, build->data, probe->length) != 0)
			return 0;
	}
	return 1;
}

static int join_prepare_build_tupdesc(const tupdesc_t *tupdesc, uint32_t natts)
{
	if(!join_session.tupdesc_valid)
	{
		join_session.build_tupdesc = *tupdesc;
		join_session.build_natts = natts;
		join_session.tupdesc_valid = 1U;
		return 0;
	}
	return join_session.build_natts == natts &&
		memcmp(&join_session.build_tupdesc, tupdesc, sizeof(*tupdesc)) == 0 ? 0 : -1;
}

static int join_insert_build(const unsigned char *tuple,
							 uint32_t tuple_len,
							 uint32_t hash,
							 const filter_tuple_value_t values[MAX_COL])
{
	uint32_t payload_capacity = join_session.workspace_bytes - join_session.payload_offset;
	uint32_t stored_bytes = join_session.inline_only ? 0U : join_align4(tuple_len);
	uint32_t index;
	uint32_t bucket;
	filter_join_entry_t *entry;

	if(tuple_len > join_session.max_tuple_bytes ||
	   join_session.entry_count >= join_session.max_build_rows ||
	   join_session.payload_used > payload_capacity ||
	   stored_bytes > payload_capacity - join_session.payload_used)
	{
		join_session.skipped_build_rows++;
		if(join_request.header)
			join_request.header->dropped_rows++;
		join_set_error(FILTER_JOIN_ERROR_WORKSPACE_FULL,
					   FILTER_JOIN_RESULT_FLAG_BUILD_FULL);
		return -1;
	}
	index = join_session.entry_count++;
	bucket = hash & (join_session.bucket_count - 1U);
	entry = &join_session.entries[index];
	entry->hash = hash;
	entry->next = join_session.buckets[bucket];
	entry->tuple_offset = join_session.inline_only ? 0U :
		join_session.payload_offset + join_session.payload_used;
	entry->tuple_bytes = join_session.inline_only ? 0U : tuple_len;
	entry->inline_key = 0U;
	if(join_session.key_count == 1U &&
	   join_session.key_type[0] != FILTER_VALUE_TYPE_BYTES)
	{
		const filter_tuple_value_t *key =
			&values[join_session.build_key_attr[0]];
		memcpy(&entry->inline_key, key->data, key->length);
		if(join_session.key_type[0] == FILTER_VALUE_TYPE_FLOAT64)
			entry->inline_key = join_canonical_float(entry->inline_key);
	}
	if(!join_session.inline_only)
		memcpy(join_session.workspace + entry->tuple_offset, tuple, tuple_len);
	join_session.payload_used += stored_bytes;
	join_session.buckets[bucket] = index + 1U;
	return 0;
}

static void join_append_match(const filter_join_entry_t *build,
							  const unsigned char *probe_tuple,
							  uint32_t probe_len,
							  uint32_t hash)
{
	filter_join_result_header_t *header = join_request.header;
	uint32_t probe_offset;
	uint32_t row_bytes;
	filter_join_row_t *row;

	header->match_rows++;
	if((join_request.config.flags & FILTER_JOIN_DESC_FLAG_COUNT_ONLY) != 0U)
		return;
	if(header->match_rows > join_request.config.max_output_rows)
	{
		header->dropped_rows++;
		header->flags |= FILTER_JOIN_RESULT_FLAG_OUTPUT_FULL;
		return;
	}
	probe_offset = join_align4(sizeof(*row) + build->tuple_bytes);
	row_bytes = join_align4(probe_offset + probe_len);
	if(join_request.write_offset > join_request.result_bytes ||
	   row_bytes > join_request.result_bytes - join_request.write_offset)
	{
		header->dropped_rows++;
		join_set_error(FILTER_JOIN_ERROR_RESULT_FULL,
					   FILTER_JOIN_RESULT_FLAG_OUTPUT_FULL);
		return;
	}
	row = (filter_join_row_t *)(join_request.result + join_request.write_offset);
	memset(row, 0, row_bytes);
	row->row_bytes = row_bytes;
	row->build_tuple_bytes = build->tuple_bytes;
	row->probe_tuple_bytes = probe_len;
	row->hash = hash;
	memcpy((unsigned char *)row + sizeof(*row),
		   join_session.workspace + build->tuple_offset, build->tuple_bytes);
	memcpy((unsigned char *)row + probe_offset, probe_tuple, probe_len);
	join_request.write_offset += row_bytes;
	header->bytes_used = join_request.write_offset;
	header->output_rows++;
}

static int join_probe(const unsigned char *probe_tuple,
						  uint32_t probe_len,
						  const filter_tuple_value_t probe_values[MAX_COL],
						  uint32_t hash)
{
	uint32_t link = join_session.buckets[hash & (join_session.bucket_count - 1U)];
	uint32_t chain = 0U;

	while(link != 0U)
	{
		uint32_t index = link - 1U;
		const filter_join_entry_t *entry;

		if(index >= join_session.entry_count)
		{
			join_set_error(FILTER_JOIN_ERROR_BAD_TUPLE,
						   FILTER_JOIN_RESULT_FLAG_BAD_TUPLE);
			return -1;
		}
		entry = &join_session.entries[index];
		chain++;
		if(entry->hash == hash)
		{
			if(join_session.key_count == 1U &&
			   join_session.key_type[0] != FILTER_VALUE_TYPE_BYTES)
			{
				const filter_tuple_value_t *probe =
					&probe_values[join_request.config.probe_key_attr[0]];
				uint64_t probe_key = 0U;

				memcpy(&probe_key, probe->data, probe->length);
				if(join_session.key_type[0] == FILTER_VALUE_TYPE_FLOAT64)
					probe_key = join_canonical_float(probe_key);
				if(probe_key == entry->inline_key)
					join_append_match(entry, probe_tuple, probe_len, hash);
				else
					join_request.header->hash_collisions++;
			}
			else
			{
				filter_tuple_value_t build_values[MAX_COL];
				uint32_t build_mask = 0U;
				int rc;

				for(uint32_t key = 0; key < join_session.key_count; key++)
					build_mask |= 1U << join_session.build_key_attr[key];
				rc = filter_tuple_decode(
					join_session.workspace + entry->tuple_offset,
					entry->tuple_bytes, &join_session.build_tupdesc,
					join_session.build_natts, build_mask, build_values);
				if(rc < 0)
				{
					join_set_error(FILTER_JOIN_ERROR_BAD_TUPLE,
								   FILTER_JOIN_RESULT_FLAG_BAD_TUPLE);
					return -1;
				}
				if(join_values_equal(probe_values, build_values))
					join_append_match(entry, probe_tuple, probe_len, hash);
				else
					join_request.header->hash_collisions++;
			}
		}
		else
			join_request.header->hash_collisions++;
		link = entry->next;
	}
	if(chain > join_request.header->max_chain)
		join_request.header->max_chain = chain;
	return 0;
}

static int join_consume(const unsigned char *page_data,
						const unsigned short *tuple_offs,
						const unsigned short *tuple_lens,
						int hit_count,
						const tupdesc_t *tupdesc,
						uint32_t natts,
						uint16_t pd_lower,
						const uint32_t *fpga_hashes)
{
	const uint8_t *attrs = join_request.config.phase == FILTER_JOIN_PHASE_BUILD ?
		join_request.config.build_key_attr : join_request.config.probe_key_attr;
	int status = 0;

	if(!filter_join_enabled() || !join_request.header || !page_data || !tuple_offs ||
	   !tuple_lens || !tupdesc || natts == 0U || natts > MAX_COL)
		return -1;
	if(join_request.config.phase == FILTER_JOIN_PHASE_BUILD &&
	   join_prepare_build_tupdesc(tupdesc, natts) < 0)
	{
		join_set_error(FILTER_JOIN_ERROR_SESSION_MISMATCH,
					   FILTER_JOIN_RESULT_FLAG_SESSION_MISS);
		return -1;
	}
	if(hit_count < 0)
		hit_count = 0;
	if(hit_count > MAX_TUPLE_PER_PAGE)
		hit_count = MAX_TUPLE_PER_PAGE;

	for(int row = 0; row < hit_count; row++)
	{
		filter_tuple_value_t values[MAX_COL];
		uint32_t tuple_off = tuple_offs[row];
		uint32_t tuple_len = tuple_lens[row];
		uint32_t hash = 0U;
		int hash_status;
		int rc;

		if(tuple_off < pd_lower || tuple_off >= PG_PAGE_SIZE || tuple_len == 0U ||
		   tuple_len > PG_PAGE_SIZE - tuple_off)
			rc = -FILTER_TUPLE_ERROR_BAD_TUPLE;
		else
			rc = filter_tuple_decode(page_data + tuple_off, tuple_len, tupdesc,
								 natts, join_request.wanted_mask, values);
		if(join_request.config.phase == FILTER_JOIN_PHASE_BUILD)
			join_request.header->build_rows++;
		else
			join_request.header->probe_rows++;
		if(rc < 0)
		{
			join_set_error(FILTER_JOIN_ERROR_BAD_TUPLE,
						   FILTER_JOIN_RESULT_FLAG_BAD_TUPLE);
			status = -1;
			continue;
		}
		if(fpga_hashes)
		{
			const filter_tuple_value_t *key = &values[attrs[0]];

			if((key->flags & FILTER_TUPLE_VALUE_FLAG_NULL) != 0U)
				hash_status = 0;
			else if(!join_value_length_valid(join_request.config.key_type[0],
										 key->length))
				hash_status = -1;
			else
			{
				hash = fpga_hashes[row];
				hash_status = 1;
			}
		}
		else
			hash_status = join_hash_values(values, attrs, &hash);
		if(hash_status <= 0)
		{
			if(hash_status < 0)
				join_set_error(FILTER_JOIN_ERROR_BAD_TUPLE,
							   FILTER_JOIN_RESULT_FLAG_BAD_TUPLE);
			continue;
		}
		if(join_request.config.phase == FILTER_JOIN_PHASE_BUILD)
			(void)join_insert_build(page_data + tuple_off, tuple_len, hash, values);
		else if(join_probe(page_data + tuple_off, tuple_len, values, hash) < 0)
			status = -1;
	}

	if(fpga_hashes)
		join_request.header->effective_mode = FILTER_JOIN_EFFECTIVE_HYBRID;
	else
	{
		if(join_request.header->effective_mode == FILTER_JOIN_EFFECTIVE_HYBRID)
			join_request.header->effective_mode = FILTER_JOIN_EFFECTIVE_MIXED;
		else if(join_request.header->effective_mode == FILTER_JOIN_EFFECTIVE_NONE)
			join_request.header->effective_mode = FILTER_JOIN_EFFECTIVE_ARM;
		if(join_request.config.mode != FILTER_JOIN_MODE_ARM)
			join_request.header->flags |= FILTER_JOIN_RESULT_FLAG_FPGA_FALLBACK;
	}
	return status;
}

int filter_join_consume_arm(const unsigned char *page_data,
							const unsigned short *tuple_offs,
							const unsigned short *tuple_lens,
							int hit_count,
							const tupdesc_t *tupdesc,
							uint32_t natts,
							uint16_t pd_lower)
{
	return join_consume(page_data, tuple_offs, tuple_lens, hit_count, tupdesc,
					natts, pd_lower, NULL);
}

int filter_join_consume_fpga_hashes(const void *staging,
								uint32_t staging_bytes,
								const unsigned char *page_data,
								const unsigned short *tuple_offs,
								const unsigned short *tuple_lens,
								int hit_count,
								const tupdesc_t *tupdesc,
								uint32_t natts,
								uint16_t pd_lower)
{
	filter_join_fpga_hash_header_t header;
	uint32_t required;

	if(!join_request.fpga_compatible || !staging || hit_count < 0)
		return -1;
	required = sizeof(header) + (uint32_t)hit_count * sizeof(uint32_t);
	if(required > staging_bytes)
		return -1;
	memcpy(&header, staging, sizeof(header));
	if(header.magic != FILTER_JOIN_FPGA_HASH_MAGIC ||
	   header.version != FILTER_JOIN_FPGA_HASH_VERSION ||
	   header.header_bytes != sizeof(header) ||
	   header.flags != 0U || header.hash_count != (uint32_t)hit_count)
	{
		join_set_error(FILTER_JOIN_ERROR_BAD_FPGA_HASHES,
					   FILTER_JOIN_RESULT_FLAG_INVALID_HASHES |
					   FILTER_JOIN_RESULT_FLAG_FPGA_FALLBACK);
		return -1;
	}
	(void)join_consume(page_data, tuple_offs, tuple_lens, hit_count, tupdesc,
				   natts, pd_lower,
				   (const uint32_t *)((const unsigned char *)staging + sizeof(header)));
	return 0;
}

void filter_join_finalize(void)
{
	uint32_t pages;

	if(!filter_join_enabled() || !join_request.header || join_request.finalized)
		return;
	join_request.header->build_rows = join_session.entry_count;
	join_request.header->workspace_used =
		(uint64_t)join_session.payload_offset + join_session.payload_used;
	pages = filter_join_result_pages();
	memcpy(join_request.result, &pages, sizeof(pages));
	join_request.finalized = 1U;
}

uint32_t filter_join_result_pages(void)
{
	uint32_t pages;

	if(!join_request.header)
		return 1U;
	pages = (join_request.header->bytes_used + PG_PAGE_SIZE - 1U) / PG_PAGE_SIZE;
	return pages ? pages : 1U;
}
