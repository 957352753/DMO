#include "filter_compute.h"

#include <stddef.h>
#include <string.h>

#include "filter_tuple.h"
#include "page.h"

#define FILTER_COMPUTE_ALIGNMENT 4U

typedef struct filter_compute_aggregate_state
{
	uint64_t nonnull_count;
	__int128 int_sum;
	double float_sum;
	double float_compensation;
	uint64_t extremum_bits;
	uint8_t initialized;
	uint8_t flags;
} filter_compute_aggregate_state_t;

typedef struct filter_compute_sort_entry
{
	uint32_t slot_offset;
	uint32_t tuple_bytes;
	uint16_t key_offset;
	uint16_t key_bytes;
	uint16_t flags;
	uint16_t reserved;
	uint64_t ordinal;
} filter_compute_sort_entry_t;

typedef struct filter_compute_runtime
{
	filter_compute_desc_t config;
	unsigned char *result;
	uint32_t result_bytes;
	filter_compute_result_header_t *header;
	unsigned char *workspace;
	uint32_t workspace_bytes;
	filter_compute_aggregate_state_t aggregate[FILTER_COMPUTE_MAX_OPS];
	filter_compute_sort_entry_t *sort_entries;
	uint32_t sort_count;
	uint32_t sort_slots_offset;
	uint64_t next_ordinal;
	uint32_t wanted_mask;
	uint32_t fpga_config;
	uint8_t fpga_compatible;
	uint8_t finalized;
} filter_compute_runtime_t;

static filter_compute_runtime_t compute_runtime;

typedef char compute_desc_size_must_be_128[
	(sizeof(filter_compute_desc_t) == 128U) ? 1 : -1];
typedef char compute_result_header_size_must_be_128[
	(sizeof(filter_compute_result_header_t) == 128U) ? 1 : -1];
typedef char compute_aggregate_value_size_must_be_40[
	(sizeof(filter_compute_aggregate_value_t) == 40U) ? 1 : -1];
typedef char compute_sort_row_size_must_be_24[
	(sizeof(filter_compute_sort_row_t) == 24U) ? 1 : -1];
typedef char compute_fpga_partial_size_must_be_64[
	(sizeof(filter_compute_fpga_partial_t) == FILTER_COMPUTE_FPGA_PARTIAL_BYTES) ? 1 : -1];

static uint32_t compute_align4(uint32_t value)
{
	return (value + FILTER_COMPUTE_ALIGNMENT - 1U) &
		   ~(FILTER_COMPUTE_ALIGNMENT - 1U);
}

static uint64_t compute_u64_words(uint32_t lo, uint32_t hi)
{
	return ((uint64_t)hi << 32) | lo;
}

static uint32_t compute_is_nan_bits(uint64_t bits)
{
	return ((bits >> 52) & 0x7ffU) == 0x7ffU &&
		   (bits & UINT64_C(0x000fffffffffffff)) != 0U;
}

static uint32_t compute_is_nonfinite_bits(uint64_t bits)
{
	return ((bits >> 52) & 0x7ffU) == 0x7ffU;
}

static int compute_float_compare(uint64_t left_bits, uint64_t right_bits)
{
	double left;
	double right;
	uint32_t left_nan = compute_is_nan_bits(left_bits);
	uint32_t right_nan = compute_is_nan_bits(right_bits);

	if(left_nan || right_nan)
	{
		if(left_nan && right_nan)
			return 0;
		return left_nan ? 1 : -1;
	}
	memcpy(&left, &left_bits, sizeof(left));
	memcpy(&right, &right_bits, sizeof(right));
	if(left < right)
		return -1;
	if(left > right)
		return 1;
	return 0;
}

static void compute_set_error(uint32_t error, uint32_t flags)
{
	if(!compute_runtime.header)
		return;
	compute_runtime.header->flags |= flags;
	if(compute_runtime.header->error_code == FILTER_COMPUTE_ERROR_NONE)
		compute_runtime.header->error_code = error;
}

static void compute_record_mode(uint32_t mode, uint64_t rows)
{
	filter_compute_result_header_t *header = compute_runtime.header;

	if(!header || rows == 0U)
		return;
	if(mode == FILTER_COMPUTE_EFFECTIVE_ARM)
		header->arm_rows += rows;
	else if(mode == FILTER_COMPUTE_EFFECTIVE_FPGA)
		header->fpga_rows += rows;

	if(header->arm_rows && header->fpga_rows)
		header->effective_mode = FILTER_COMPUTE_EFFECTIVE_MIXED;
	else
		header->effective_mode = (uint8_t)mode;
	if(mode == FILTER_COMPUTE_EFFECTIVE_ARM &&
	   compute_runtime.config.mode != FILTER_COMPUTE_MODE_ARM)
		header->flags |= FILTER_COMPUTE_RESULT_FLAG_FPGA_FALLBACK;
}

void filter_compute_set_workspace(void *workspace, uint32_t workspace_bytes)
{
	compute_runtime.workspace = (unsigned char *)workspace;
	compute_runtime.workspace_bytes = workspace_bytes;
}

void filter_compute_disable(void)
{
	unsigned char *workspace = compute_runtime.workspace;
	uint32_t workspace_bytes = compute_runtime.workspace_bytes;

	memset(&compute_runtime, 0, sizeof(compute_runtime));
	compute_runtime.workspace = workspace;
	compute_runtime.workspace_bytes = workspace_bytes;
}

static uint32_t compute_type_valid(uint32_t type, uint32_t allow_bytes)
{
	return type == FILTER_VALUE_TYPE_INT32 ||
		   type == FILTER_VALUE_TYPE_INT64 ||
		   type == FILTER_VALUE_TYPE_FLOAT64 ||
		   (allow_bytes && type == FILTER_VALUE_TYPE_BYTES);
}

static void compute_select_fpga_path(void)
{
	const filter_compute_op_desc_t *first = &compute_runtime.config.ops[0];
	uint32_t mask = 0U;

	compute_runtime.fpga_compatible = 0U;
	compute_runtime.fpga_config = 0U;
	if(compute_runtime.config.kind != FILTER_COMPUTE_KIND_AGGREGATE ||
	   compute_runtime.config.mode == FILTER_COMPUTE_MODE_ARM)
		return;

	for(uint32_t i = 0; i < compute_runtime.config.op_count; i++)
	{
		const filter_compute_op_desc_t *op = &compute_runtime.config.ops[i];

		if(op->attr_index != first->attr_index || op->value_type != first->value_type)
			return;
		if(op->opcode == FILTER_COMPUTE_OP_SUM)
			mask |= FILTER_COMPUTE_FPGA_OP_SUM;
		else if(op->opcode == FILTER_COMPUTE_OP_MIN)
			mask |= FILTER_COMPUTE_FPGA_OP_MIN;
		else if(op->opcode == FILTER_COMPUTE_OP_MAX)
			mask |= FILTER_COMPUTE_FPGA_OP_MAX;
		else
			return;
	}

	if(first->value_type == FILTER_VALUE_TYPE_INT32)
		;
	else if((first->value_type == FILTER_VALUE_TYPE_INT64 ||
			 first->value_type == FILTER_VALUE_TYPE_FLOAT64) &&
			(mask & FILTER_COMPUTE_FPGA_OP_SUM) == 0U)
		;
	else
		return;

	compute_runtime.fpga_compatible = 1U;
	/* [3:0] kind, [7:4] type, [15:8] attr, [23:16] operation mask. */
	compute_runtime.fpga_config = 1U |
		(first->value_type << 4) |
		((uint32_t)first->attr_index << 8) |
		(mask << 16);
}

int filter_compute_configure(const filter_compute_desc_t *desc,
							 uint32_t natts,
							 uint32_t count_only)
{
	filter_compute_desc_t config;

	filter_tuple_reset_decoder_cache();
	filter_compute_disable();
	if(!desc || desc->magic != FILTER_COMPUTE_DESC_MAGIC)
		return 0;
	memcpy(&config, desc, sizeof(config));
	if(config.version != FILTER_COMPUTE_DESC_VERSION ||
	   config.struct_bytes < sizeof(config) || count_only ||
	   natts == 0U || natts > MAX_COL ||
	   (config.mode != FILTER_COMPUTE_MODE_ARM &&
		config.mode != FILTER_COMPUTE_MODE_FPGA &&
		config.mode != FILTER_COMPUTE_MODE_AUTO) ||
	   (config.kind != FILTER_COMPUTE_KIND_AGGREGATE &&
		config.kind != FILTER_COMPUTE_KIND_SORT) ||
	   config.op_count == 0U || config.op_count > FILTER_COMPUTE_MAX_OPS)
		return -1;

	if(config.kind == FILTER_COMPUTE_KIND_SORT)
	{
		const filter_compute_op_desc_t *op = &config.ops[0];

		if(config.op_count != 1U || op->opcode != FILTER_COMPUTE_OP_SORT ||
		   !compute_type_valid(op->value_type, 1U) || op->attr_index >= natts ||
		   (op->flags & ~(FILTER_COMPUTE_OP_FLAG_DESC |
						 FILTER_COMPUTE_OP_FLAG_NULLS_FIRST)) != 0U ||
		   config.sort_limit == 0U || config.sort_limit > FILTER_COMPUTE_MAX_SORT_ROWS ||
		   config.max_tuple_bytes < 23U ||
		   config.max_tuple_bytes > FILTER_COMPUTE_MAX_TUPLE_BYTES)
			return -1;
	}
	else
	{
		for(uint32_t i = 0; i < config.op_count; i++)
		{
			const filter_compute_op_desc_t *op = &config.ops[i];

			if((op->opcode != FILTER_COMPUTE_OP_SUM &&
				op->opcode != FILTER_COMPUTE_OP_MIN &&
				op->opcode != FILTER_COMPUTE_OP_MAX) ||
			   !compute_type_valid(op->value_type, 0U) ||
			   op->attr_index >= natts || op->flags != 0U)
				return -1;
		}
	}

	compute_runtime.config = config;
	for(uint32_t i = 0; i < config.op_count; i++)
		compute_runtime.wanted_mask |= 1U << config.ops[i].attr_index;
	compute_select_fpga_path();
	return 1;
}

void filter_compute_reset_result(void *result_buffer, uint32_t result_bytes)
{
	filter_compute_result_header_t *header;
	uint32_t page_count = 1U;

	if(!filter_compute_enabled() || !result_buffer ||
	   result_bytes < FILTER_COMPUTE_RESULT_DATA_OFFSET)
		return;
	memset(result_buffer, 0, result_bytes);
	compute_runtime.result = (unsigned char *)result_buffer;
	compute_runtime.result_bytes = result_bytes;
	compute_runtime.finalized = 0U;
	memcpy(compute_runtime.result, &page_count, sizeof(page_count));
	header = (filter_compute_result_header_t *)
		(compute_runtime.result + FILTER_COMPUTE_RESULT_HEADER_OFFSET);
	compute_runtime.header = header;
	header->magic = FILTER_COMPUTE_RESULT_MAGIC;
	header->version = FILTER_COMPUTE_RESULT_VERSION;
	header->header_bytes = sizeof(*header);
	header->requested_mode = compute_runtime.config.mode;
	header->kind = compute_runtime.config.kind;
	header->op_count = compute_runtime.config.op_count;
	header->bytes_used = FILTER_COMPUTE_RESULT_DATA_OFFSET;
	header->sort_limit = compute_runtime.config.sort_limit;
	header->max_tuple_bytes = compute_runtime.config.max_tuple_bytes;
	memcpy(header->ops, compute_runtime.config.ops, sizeof(header->ops));

	if(compute_runtime.config.kind == FILTER_COMPUTE_KIND_SORT)
	{
		uint64_t entries_bytes = (uint64_t)compute_runtime.config.sort_limit *
			sizeof(filter_compute_sort_entry_t);
		uint64_t slots_bytes = (uint64_t)compute_runtime.config.sort_limit *
			compute_runtime.config.max_tuple_bytes;
		uint64_t required = compute_align4((uint32_t)entries_bytes) + slots_bytes;

		if(!compute_runtime.workspace || entries_bytes > UINT32_MAX ||
		   required > compute_runtime.workspace_bytes)
		{
			compute_set_error(FILTER_COMPUTE_ERROR_WORKSPACE_FULL,
						  FILTER_COMPUTE_RESULT_FLAG_FULL);
			return;
		}
		compute_runtime.sort_entries =
			(filter_compute_sort_entry_t *)compute_runtime.workspace;
		compute_runtime.sort_slots_offset = compute_align4((uint32_t)entries_bytes);
		memset(compute_runtime.sort_entries, 0, (size_t)entries_bytes);
	}
}

uint32_t filter_compute_enabled(void)
{
	return compute_runtime.config.mode != FILTER_COMPUTE_MODE_DISABLED;
}

uint32_t filter_compute_kind(void)
{
	return compute_runtime.config.kind;
}

uint32_t filter_compute_mode(void)
{
	return compute_runtime.config.mode;
}

uint32_t filter_compute_fpga_enabled(void)
{
	return compute_runtime.fpga_compatible;
}

uint32_t filter_compute_fpga_config(void)
{
	return compute_runtime.fpga_compatible ? compute_runtime.fpga_config : 0U;
}

static int compute_value_length_valid(uint32_t type, uint32_t length)
{
	if(type == FILTER_VALUE_TYPE_INT32)
		return length == 4U;
	if(type == FILTER_VALUE_TYPE_INT64 || type == FILTER_VALUE_TYPE_FLOAT64)
		return length == 8U;
	return type == FILTER_VALUE_TYPE_BYTES;
}

static void compute_aggregate_update(uint32_t op_index,
								 const filter_tuple_value_t *value)
{
	const filter_compute_op_desc_t *op = &compute_runtime.config.ops[op_index];
	filter_compute_aggregate_state_t *state = &compute_runtime.aggregate[op_index];
	uint64_t bits = 0U;

	if((value->flags & FILTER_TUPLE_VALUE_FLAG_NULL) != 0U)
		return;
	if(!compute_value_length_valid(op->value_type, value->length))
	{
		compute_set_error(FILTER_COMPUTE_ERROR_BAD_TUPLE,
						  FILTER_COMPUTE_RESULT_FLAG_BAD_TUPLE);
		return;
	}
	memcpy(&bits, value->data, value->length);
	state->nonnull_count++;

	if(op->opcode == FILTER_COMPUTE_OP_SUM)
	{
		if(op->value_type == FILTER_VALUE_TYPE_INT32)
		{
			int32_t input;
			memcpy(&input, value->data, sizeof(input));
			state->int_sum += input;
		}
		else if(op->value_type == FILTER_VALUE_TYPE_INT64)
		{
			int64_t input;
			memcpy(&input, value->data, sizeof(input));
			state->int_sum += input;
		}
		else
		{
			double input;
			double corrected;
			double next;
			uint64_t sum_bits;

			memcpy(&input, value->data, sizeof(input));
			memcpy(&sum_bits, &state->float_sum, sizeof(sum_bits));
			if(compute_is_nonfinite_bits(bits) ||
			   compute_is_nonfinite_bits(sum_bits))
			{
				state->float_sum += input;
				state->float_compensation = 0.0;
			}
			else
			{
				corrected = input - state->float_compensation;
				next = state->float_sum + corrected;
				state->float_compensation =
					(next - state->float_sum) - corrected;
				state->float_sum = next;
			}
			memcpy(&sum_bits, &state->float_sum, sizeof(sum_bits));
			if(compute_is_nan_bits(sum_bits))
				state->flags |= FILTER_COMPUTE_VALUE_FLAG_NAN;
		}
		state->initialized = 1U;
		return;
	}

	if(!state->initialized)
	{
		state->extremum_bits = bits;
		state->initialized = 1U;
	}
	else
	{
		int compare;

		if(op->value_type == FILTER_VALUE_TYPE_INT32)
		{
			int32_t left = (int32_t)(uint32_t)bits;
			int32_t right = (int32_t)(uint32_t)state->extremum_bits;
			compare = (left > right) - (left < right);
		}
		else if(op->value_type == FILTER_VALUE_TYPE_INT64)
		{
			int64_t left = (int64_t)bits;
			int64_t right = (int64_t)state->extremum_bits;
			compare = (left > right) - (left < right);
		}
		else
		{
			compare = compute_float_compare(bits, state->extremum_bits);
			if(compute_is_nan_bits(bits))
				state->flags |= FILTER_COMPUTE_VALUE_FLAG_NAN;
		}

		if((op->opcode == FILTER_COMPUTE_OP_MIN && compare < 0) ||
		   (op->opcode == FILTER_COMPUTE_OP_MAX && compare > 0))
			state->extremum_bits = bits;
	}
}

static const unsigned char *compute_sort_tuple(const filter_compute_sort_entry_t *entry)
{
	return compute_runtime.workspace + entry->slot_offset;
}

static int compute_raw_key_compare(uint32_t type,
							   const unsigned char *left, uint32_t left_len,
							   const unsigned char *right, uint32_t right_len)
{
	if(type == FILTER_VALUE_TYPE_INT32)
	{
		int32_t a;
		int32_t b;
		if(left_len != 4U || right_len != 4U)
			return (left_len > right_len) - (left_len < right_len);
		memcpy(&a, left, sizeof(a));
		memcpy(&b, right, sizeof(b));
		return (a > b) - (a < b);
	}
	if(type == FILTER_VALUE_TYPE_INT64)
	{
		int64_t a;
		int64_t b;
		if(left_len != 8U || right_len != 8U)
			return (left_len > right_len) - (left_len < right_len);
		memcpy(&a, left, sizeof(a));
		memcpy(&b, right, sizeof(b));
		return (a > b) - (a < b);
	}
	if(type == FILTER_VALUE_TYPE_FLOAT64)
	{
		uint64_t a;
		uint64_t b;
		if(left_len != 8U || right_len != 8U)
			return (left_len > right_len) - (left_len < right_len);
		memcpy(&a, left, sizeof(a));
		memcpy(&b, right, sizeof(b));
		return compute_float_compare(a, b);
	}
	{
		uint32_t common = left_len < right_len ? left_len : right_len;
		int compare = common ? memcmp(left, right, common) : 0;

		if(compare < 0)
			return -1;
		if(compare > 0)
			return 1;
		return (left_len > right_len) - (left_len < right_len);
	}
}

static int compute_sort_compare_values(const unsigned char *left,
								   uint32_t left_len,
								   uint32_t left_null,
								   uint64_t left_ordinal,
								   const filter_compute_sort_entry_t *right)
{
	const filter_compute_op_desc_t *op = &compute_runtime.config.ops[0];
	uint32_t right_null = (right->flags & FILTER_COMPUTE_SORT_ROW_FLAG_KEY_NULL) != 0U;
	int compare;

	if(left_null || right_null)
	{
		if(left_null && right_null)
			compare = 0;
		else if((op->flags & FILTER_COMPUTE_OP_FLAG_NULLS_FIRST) != 0U)
			compare = left_null ? -1 : 1;
		else
			compare = left_null ? 1 : -1;
	}
	else
	{
		const unsigned char *right_tuple = compute_sort_tuple(right);
		compare = compute_raw_key_compare(op->value_type, left, left_len,
									  right_tuple + right->key_offset,
									  right->key_bytes);
		if((op->flags & FILTER_COMPUTE_OP_FLAG_DESC) != 0U)
			compare = -compare;
	}

	if(compare == 0)
		compare = (left_ordinal > right->ordinal) - (left_ordinal < right->ordinal);
	return compare;
}

static int compute_sort_compare_entries(const filter_compute_sort_entry_t *left,
									const filter_compute_sort_entry_t *right)
{
	const unsigned char *tuple = compute_sort_tuple(left);
	return compute_sort_compare_values(tuple + left->key_offset,
								   left->key_bytes,
								   (left->flags & FILTER_COMPUTE_SORT_ROW_FLAG_KEY_NULL) != 0U,
								   left->ordinal, right);
}

static void compute_sort_swap(uint32_t left, uint32_t right)
{
	filter_compute_sort_entry_t temp = compute_runtime.sort_entries[left];
	compute_runtime.sort_entries[left] = compute_runtime.sort_entries[right];
	compute_runtime.sort_entries[right] = temp;
}

static void compute_sort_sift_down(uint32_t root, uint32_t count)
{
	for(;;)
	{
		uint32_t child = root * 2U + 1U;

		if(child >= count)
			return;
		if(child + 1U < count &&
		   compute_sort_compare_entries(&compute_runtime.sort_entries[child],
									&compute_runtime.sort_entries[child + 1U]) < 0)
			child++;
		if(compute_sort_compare_entries(&compute_runtime.sort_entries[root],
									&compute_runtime.sort_entries[child]) >= 0)
			return;
		compute_sort_swap(root, child);
		root = child;
	}
}

static void compute_sort_sift_up(uint32_t child)
{
	while(child != 0U)
	{
		uint32_t parent = (child - 1U) >> 1;

		if(compute_sort_compare_entries(&compute_runtime.sort_entries[parent],
									&compute_runtime.sort_entries[child]) >= 0)
			return;
		compute_sort_swap(parent, child);
		child = parent;
	}
}

static void compute_sort_store(uint32_t index,
							   const unsigned char *tuple,
							   uint32_t tuple_len,
							   const filter_tuple_value_t *key,
							   uint64_t ordinal)
{
	filter_compute_sort_entry_t *entry = &compute_runtime.sort_entries[index];
	uint32_t slot_offset = entry->slot_offset;

	if(slot_offset == 0U)
		slot_offset = compute_runtime.sort_slots_offset +
			index * compute_runtime.config.max_tuple_bytes;

	memcpy(compute_runtime.workspace + slot_offset, tuple, tuple_len);
	memset(entry, 0, sizeof(*entry));
	entry->slot_offset = slot_offset;
	entry->tuple_bytes = tuple_len;
	entry->ordinal = ordinal;
	if((key->flags & FILTER_TUPLE_VALUE_FLAG_NULL) != 0U)
		entry->flags = FILTER_COMPUTE_SORT_ROW_FLAG_KEY_NULL;
	else
	{
		entry->key_offset = (uint16_t)(key->data - tuple);
		entry->key_bytes = key->length;
	}
}

static void compute_sort_consume(const unsigned char *tuple,
								 uint32_t tuple_len,
								 const filter_tuple_value_t *key)
{
	uint64_t ordinal = compute_runtime.next_ordinal++;
	uint32_t is_null = (key->flags & FILTER_TUPLE_VALUE_FLAG_NULL) != 0U;

	if(tuple_len > compute_runtime.config.max_tuple_bytes ||
	   (!is_null && !compute_value_length_valid(
		compute_runtime.config.ops[0].value_type, key->length)))
	{
		compute_set_error(FILTER_COMPUTE_ERROR_BAD_TUPLE,
						  FILTER_COMPUTE_RESULT_FLAG_BAD_TUPLE);
		return;
	}

	if(compute_runtime.sort_count < compute_runtime.config.sort_limit)
	{
		uint32_t index = compute_runtime.sort_count++;
		compute_sort_store(index, tuple, tuple_len, key, ordinal);
		compute_sort_sift_up(index);
		return;
	}

	compute_runtime.header->flags |= FILTER_COMPUTE_RESULT_FLAG_TRUNCATED;
	if(compute_sort_compare_values(key->data, key->length, is_null,
								 ordinal, &compute_runtime.sort_entries[0]) < 0)
	{
		compute_sort_store(0U, tuple, tuple_len, key, ordinal);
		compute_sort_sift_down(0U, compute_runtime.sort_count);
	}
}

int filter_compute_consume_arm(const unsigned char *page_data,
							   const unsigned short *tuple_offs,
							   const unsigned short *tuple_lens,
							   int hit_count,
							   const tupdesc_t *tupdesc,
							   uint32_t natts,
							   uint16_t pd_lower)
{
	uint64_t consumed = 0U;
	int status = 0;

	if(!filter_compute_enabled() || !compute_runtime.header || !page_data ||
	   !tuple_offs || !tuple_lens || !tupdesc || natts == 0U || natts > MAX_COL)
		return -1;
	if(hit_count < 0)
		hit_count = 0;
	if(hit_count > MAX_TUPLE_PER_PAGE)
		hit_count = MAX_TUPLE_PER_PAGE;

	for(int row = 0; row < hit_count; row++)
	{
		filter_tuple_value_t values[MAX_COL];
		uint32_t tuple_off = tuple_offs[row];
		uint32_t tuple_len = tuple_lens[row];
		int rc;

		if(tuple_off < pd_lower || tuple_off >= PG_PAGE_SIZE || tuple_len == 0U ||
		   tuple_len > PG_PAGE_SIZE - tuple_off)
			rc = -FILTER_TUPLE_ERROR_BAD_TUPLE;
		else
			rc = filter_tuple_decode(page_data + tuple_off, tuple_len, tupdesc,
								 natts, compute_runtime.wanted_mask, values);
		compute_runtime.header->input_rows++;
		consumed++;
		if(rc < 0)
		{
			compute_set_error(FILTER_COMPUTE_ERROR_BAD_TUPLE,
							  FILTER_COMPUTE_RESULT_FLAG_BAD_TUPLE);
			status = -1;
			continue;
		}

		if(compute_runtime.config.kind == FILTER_COMPUTE_KIND_AGGREGATE)
		{
			for(uint32_t op = 0; op < compute_runtime.config.op_count; op++)
				compute_aggregate_update(op,
					&values[compute_runtime.config.ops[op].attr_index]);
		}
		else if(compute_runtime.sort_entries)
		{
			compute_sort_consume(page_data + tuple_off, tuple_len,
				&values[compute_runtime.config.ops[0].attr_index]);
		}
	}

	compute_record_mode(FILTER_COMPUTE_EFFECTIVE_ARM, consumed);
	return status;
}

static void compute_merge_extremum(uint32_t op_index, uint64_t bits)
{
	const filter_compute_op_desc_t *op = &compute_runtime.config.ops[op_index];
	filter_compute_aggregate_state_t *state = &compute_runtime.aggregate[op_index];
	int compare;

	if(!state->initialized)
	{
		state->extremum_bits = bits;
		state->initialized = 1U;
		return;
	}
	if(op->value_type == FILTER_VALUE_TYPE_INT32)
	{
		int32_t left = (int32_t)(uint32_t)bits;
		int32_t right = (int32_t)(uint32_t)state->extremum_bits;
		compare = (left > right) - (left < right);
	}
	else if(op->value_type == FILTER_VALUE_TYPE_INT64)
	{
		int64_t left = (int64_t)bits;
		int64_t right = (int64_t)state->extremum_bits;
		compare = (left > right) - (left < right);
	}
	else
		compare = compute_float_compare(bits, state->extremum_bits);
	if((op->opcode == FILTER_COMPUTE_OP_MIN && compare < 0) ||
	   (op->opcode == FILTER_COMPUTE_OP_MAX && compare > 0))
		state->extremum_bits = bits;
}

int filter_compute_merge_fpga(const void *staging,
							  uint32_t staging_bytes,
							  uint32_t hit_count)
{
	filter_compute_fpga_partial_t partial;
	uint64_t nonnull;
	uint64_t nulls;
	uint64_t sum_bits;
	uint64_t min_bits;
	uint64_t max_bits;
	const filter_compute_op_desc_t *first = &compute_runtime.config.ops[0];

	if(!compute_runtime.fpga_compatible || !compute_runtime.header || !staging ||
	   staging_bytes < sizeof(partial))
		return -1;
	memcpy(&partial, staging, sizeof(partial));
	if(partial.magic != FILTER_COMPUTE_FPGA_PARTIAL_MAGIC ||
	   partial.version != FILTER_COMPUTE_FPGA_PARTIAL_VERSION ||
	   partial.struct_bytes != sizeof(partial) ||
	   partial.value_type != first->value_type ||
	   partial.attr_index != first->attr_index ||
	   partial.op_mask != (uint8_t)(compute_runtime.fpga_config >> 16))
	{
		compute_set_error(FILTER_COMPUTE_ERROR_BAD_FPGA_PARTIAL,
						  FILTER_COMPUTE_RESULT_FLAG_INVALID_STAGE |
						  FILTER_COMPUTE_RESULT_FLAG_FPGA_FALLBACK);
		return -1;
	}

	nonnull = compute_u64_words(partial.nonnull_lo, partial.nonnull_hi);
	nulls = compute_u64_words(partial.null_lo, partial.null_hi);
	if(nonnull + nulls != hit_count)
	{
		compute_set_error(FILTER_COMPUTE_ERROR_BAD_FPGA_PARTIAL,
						  FILTER_COMPUTE_RESULT_FLAG_INVALID_STAGE |
						  FILTER_COMPUTE_RESULT_FLAG_FPGA_FALLBACK);
		return -1;
	}
	sum_bits = compute_u64_words(partial.sum_lo, partial.sum_hi);
	min_bits = compute_u64_words(partial.min_lo, partial.min_hi);
	max_bits = compute_u64_words(partial.max_lo, partial.max_hi);
	for(uint32_t i = 0; i < compute_runtime.config.op_count; i++)
	{
		filter_compute_aggregate_state_t *state = &compute_runtime.aggregate[i];
		const filter_compute_op_desc_t *op = &compute_runtime.config.ops[i];

		state->nonnull_count += nonnull;
		if(nonnull == 0U)
			continue;
		if(op->opcode == FILTER_COMPUTE_OP_SUM)
		{
			state->int_sum += (int64_t)sum_bits;
			state->initialized = 1U;
		}
		else if(op->opcode == FILTER_COMPUTE_OP_MIN)
			compute_merge_extremum(i, min_bits);
		else
			compute_merge_extremum(i, max_bits);
	}
	if(partial.flags != 0U)
	{
		for(uint32_t i = 0; i < compute_runtime.config.op_count; i++)
		{
			if(compute_runtime.config.ops[i].opcode == FILTER_COMPUTE_OP_SUM)
				compute_runtime.aggregate[i].flags |=
					FILTER_COMPUTE_VALUE_FLAG_OVERFLOW;
		}
		compute_set_error(FILTER_COMPUTE_ERROR_NONE,
						  FILTER_COMPUTE_RESULT_FLAG_OVERFLOW);
	}
	compute_runtime.header->input_rows += hit_count;
	compute_record_mode(FILTER_COMPUTE_EFFECTIVE_FPGA, hit_count);
	return 0;
}

static void compute_finalize_aggregate(void)
{
	filter_compute_aggregate_value_t *values =
		(filter_compute_aggregate_value_t *)(compute_runtime.result +
			FILTER_COMPUTE_RESULT_DATA_OFFSET);
	uint32_t bytes = compute_runtime.config.op_count * sizeof(*values);

	if(bytes > compute_runtime.result_bytes - FILTER_COMPUTE_RESULT_DATA_OFFSET)
	{
		compute_set_error(FILTER_COMPUTE_ERROR_RESULT_FULL,
						  FILTER_COMPUTE_RESULT_FLAG_FULL);
		return;
	}
	memset(values, 0, bytes);
	for(uint32_t i = 0; i < compute_runtime.config.op_count; i++)
	{
		const filter_compute_op_desc_t *op = &compute_runtime.config.ops[i];
		filter_compute_aggregate_state_t *state = &compute_runtime.aggregate[i];
		filter_compute_aggregate_value_t *out = &values[i];

		out->opcode = op->opcode;
		out->value_type = op->value_type;
		out->attr_index = op->attr_index;
		out->nonnull_count = state->nonnull_count;
		if(state->nonnull_count == 0U)
		{
			out->flags = FILTER_COMPUTE_VALUE_FLAG_NULL;
			continue;
		}
		if(op->opcode == FILTER_COMPUTE_OP_SUM)
		{
			if(op->value_type == FILTER_VALUE_TYPE_INT32)
			{
				int64_t sum = (int64_t)state->int_sum;
				out->value_bytes = sizeof(sum);
				memcpy(out->value, &sum, sizeof(sum));
			}
			else if(op->value_type == FILTER_VALUE_TYPE_INT64)
			{
				out->value_bytes = 16U;
				memcpy(out->value, &state->int_sum, 16U);
			}
			else
			{
				out->value_bytes = sizeof(state->float_sum);
				memcpy(out->value, &state->float_sum, sizeof(state->float_sum));
			}
		}
		else
		{
			out->value_bytes = op->value_type == FILTER_VALUE_TYPE_INT32 ? 4U : 8U;
			memcpy(out->value, &state->extremum_bits, out->value_bytes);
		}
		out->flags |= state->flags;
		if((state->flags & FILTER_COMPUTE_VALUE_FLAG_NAN) != 0U)
			compute_runtime.header->flags |= FILTER_COMPUTE_RESULT_FLAG_NAN;
	}
	compute_runtime.header->output_rows = compute_runtime.config.op_count;
	compute_runtime.header->bytes_used = FILTER_COMPUTE_RESULT_DATA_OFFSET + bytes;
}

static void compute_finalize_sort(void)
{
	uint32_t cursor = FILTER_COMPUTE_RESULT_DATA_OFFSET;

	if(!compute_runtime.sort_entries)
		return;
	for(uint32_t end = compute_runtime.sort_count; end > 1U; end--)
	{
		compute_sort_swap(0U, end - 1U);
		compute_sort_sift_down(0U, end - 1U);
	}

	for(uint32_t i = 0; i < compute_runtime.sort_count; i++)
	{
		const filter_compute_sort_entry_t *entry = &compute_runtime.sort_entries[i];
		uint32_t row_bytes = compute_align4(sizeof(filter_compute_sort_row_t) +
			entry->tuple_bytes);
		filter_compute_sort_row_t *row;

		if(cursor > compute_runtime.result_bytes ||
		   row_bytes > compute_runtime.result_bytes - cursor)
		{
			compute_set_error(FILTER_COMPUTE_ERROR_RESULT_FULL,
							  FILTER_COMPUTE_RESULT_FLAG_FULL |
							  FILTER_COMPUTE_RESULT_FLAG_TRUNCATED);
			break;
		}
		row = (filter_compute_sort_row_t *)(compute_runtime.result + cursor);
		memset(row, 0, row_bytes);
		row->row_bytes = row_bytes;
		row->tuple_bytes = entry->tuple_bytes;
		row->key_offset = entry->key_offset;
		row->key_bytes = entry->key_bytes;
		row->flags = entry->flags;
		row->ordinal = entry->ordinal;
		memcpy((unsigned char *)row + sizeof(*row), compute_sort_tuple(entry),
			   entry->tuple_bytes);
		cursor += row_bytes;
		compute_runtime.header->output_rows++;
	}
	compute_runtime.header->bytes_used = cursor;
}

void filter_compute_finalize(void)
{
	uint32_t pages;

	if(!filter_compute_enabled() || !compute_runtime.header ||
	   compute_runtime.finalized)
		return;
	if(compute_runtime.config.kind == FILTER_COMPUTE_KIND_AGGREGATE)
		compute_finalize_aggregate();
	else
		compute_finalize_sort();
	pages = filter_compute_result_pages();
	memcpy(compute_runtime.result, &pages, sizeof(pages));
	compute_runtime.finalized = 1U;
}

uint32_t filter_compute_result_pages(void)
{
	uint32_t bytes;
	uint32_t pages;

	if(!compute_runtime.header)
		return 1U;
	bytes = compute_runtime.header->bytes_used;
	pages = (bytes + PG_PAGE_SIZE - 1U) / PG_PAGE_SIZE;
	return pages ? pages : 1U;
}
