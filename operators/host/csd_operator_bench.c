#define _GNU_SOURCE

#include "csd_canonical_checksum.h"
#include "csd_operator_io.h"
#include "filter_compute_client.h"
#include "filter_join_client.h"
#include "filter_projection_client.h"

#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__linux__) && !defined(CSD_OPERATOR_IO_MOCK)
#include <sys/resource.h>
#include <unistd.h>
#else
#ifdef _WIN32
#include <malloc.h>
#include <io.h>
#define close _close
#else
#include <unistd.h>
#endif
#endif

#define BENCH_MAX_FILES CSD_OPERATOR_MAX_FILES
#define BENCH_MAX_TOP_KEYS 1000000U
#define BENCH_ERROR_BYTES 512U

typedef enum bench_operation
{
	BENCH_OPERATION_NONE = 0,
	BENCH_OPERATION_PROJECTION,
	BENCH_OPERATION_AGGREGATE,
	BENCH_OPERATION_TOPK,
	BENCH_OPERATION_JOIN
} bench_operation_t;

typedef struct bench_file_list
{
	const char *items[BENCH_MAX_FILES];
	size_t count;
} bench_file_list_t;

typedef struct bench_config
{
	bench_operation_t operation;
	const char *device;
	bench_file_list_t files;
	bench_file_list_t build_files;
	bench_file_list_t probe_files;
	const char *desc_spec;
	const char *build_desc_spec;
	const char *probe_desc_spec;
	const char *mode;
	const char *label;
	const char *attrs;
	const char *ops;
	const char *key_type;
	uint32_t key_attr;
	uint32_t build_key_attr;
	uint32_t probe_key_attr;
	uint32_t split_lbas;
	uint32_t result_pages;
	uint32_t timeout_ms;
	uint32_t sort_limit;
	uint32_t max_tuple_bytes;
	uint32_t sort_descending;
	uint32_t nulls_first;
	uint32_t bucket_count;
	uint32_t max_build_rows;
	uint32_t session_id;
	uint32_t hash_seed;
	uint32_t trace_commands;
} bench_config_t;

typedef struct bench_cpu_time
{
	double user_ms;
	double system_ms;
} bench_cpu_time_t;

typedef struct bench_run_stats
{
	uint64_t commands;
	uint64_t input_lbas;
	uint64_t input_rows;
	uint64_t output_rows;
	uint64_t result_bytes_used;
	uint64_t result_dma_bytes;
	uint64_t arm_commands;
	uint64_t fpga_commands;
	uint64_t mixed_commands;
	uint64_t fallback_commands;
	uint32_t flags_or;
	double elapsed_ms;
	double setup_ms;
	double scan_execute_merge_ms;
	double cpu_user_ms;
	double cpu_system_ms;
} bench_run_stats_t;

typedef struct aggregate_state
{
	filter_compute_op_desc_t op;
	uint64_t nonnull_count;
	uint32_t flags;
	uint32_t has_value;
	int64_t int32_sum;
	__int128 int64_sum;
	double float_sum;
	uint8_t extremum[8];
} aggregate_state_t;

typedef struct top_key
{
	uint8_t *data;
	uint32_t length;
	uint32_t is_null;
	uint64_t sequence;
} top_key_t;

typedef struct top_key_list
{
	top_key_t *items;
	size_t count;
	size_t capacity;
} top_key_list_t;

static uint8_t top_compare_type;
static uint8_t top_compare_flags;

static void usage(FILE *stream)
{
	fputs(
		"Usage:\n"
		"  csd_operator_bench projection --device DEV --file PATH [--file PATH ...]\\\n\n"
		"      --desc LEN:ALIGN,... --mode arm|fpga --attrs INDEX,...\n"
		"  csd_operator_bench aggregate --device DEV --file PATH [--file PATH ...]\\\n\n"
		"      --desc LEN:ALIGN,... --mode arm|fpga|auto\\\n\n"
		"      --ops sum|minimum|maximum:TYPE:INDEX[,..]\n"
		"  csd_operator_bench topk --device DEV --file PATH [--file PATH ...]\\\n\n"
		"      --desc LEN:ALIGN,... --mode arm|auto --key-type TYPE --key-attr INDEX\\\n\n"
		"      --limit K [--descending] [--nulls-first]\n"
		"  csd_operator_bench join --device DEV --build-file PATH [..] --probe-file PATH [..]\\\n\n"
		"      --build-desc LEN:ALIGN,... --probe-desc LEN:ALIGN,...\\\n\n"
		"      --mode arm|hybrid|auto --key-type TYPE --build-key INDEX --probe-key INDEX\\\n\n"
		"      --buckets POWER_OF_TWO --max-build-rows N\n"
		"Common: --split-lbas N --result-pages 1..32 --timeout-ms N --max-tuple-bytes N [--trace-commands]\n"
		"TYPE is int4, int8, float8, or bytes where supported. Attribute indexes are zero-based.\n",
		stream);
}

static int parse_u32(const char *text, uint32_t *value)
{
	char *end = NULL;
	unsigned long parsed;

	if(!text || *text == '\0')
		return -1;
	errno = 0;
	parsed = strtoul(text, &end, 0);
	if(errno != 0 || end == text || *end != '\0' || parsed > UINT32_MAX)
		return -1;
	*value = (uint32_t)parsed;
	return 0;
}

static char *next_token(char **cursor, char delimiter)
{
	char *token;
	char *separator;
	if(!cursor || !*cursor)
		return NULL;
	token = *cursor;
	separator = strchr(token, delimiter);
	if(separator)
	{
		*separator = '\0';
		*cursor = separator + 1;
	}
	else
		*cursor = NULL;
	return token;
}

static int add_file(bench_file_list_t *files, const char *path)
{
	if(!files || !path || *path == '\0' || files->count >= BENCH_MAX_FILES)
		return -1;
	files->items[files->count++] = path;
	return 0;
}

static int option_value(int argc, char **argv, int *index, const char **value)
{
	if(*index + 1 >= argc)
		return -1;
	(*index)++;
	*value = argv[*index];
	return 0;
}

static int parse_arguments(int argc, char **argv, bench_config_t *config)
{
	int i;

	if(argc < 2 || !config)
		return -1;
	memset(config, 0, sizeof(*config));
	if(strcmp(argv[1], "projection") == 0)
		config->operation = BENCH_OPERATION_PROJECTION;
	else if(strcmp(argv[1], "aggregate") == 0)
		config->operation = BENCH_OPERATION_AGGREGATE;
	else if(strcmp(argv[1], "topk") == 0)
		config->operation = BENCH_OPERATION_TOPK;
	else if(strcmp(argv[1], "join") == 0)
		config->operation = BENCH_OPERATION_JOIN;
	else
		return -1;

	config->split_lbas = 8192U;
	config->result_pages = 32U;
	config->timeout_ms = 60000U;
	config->sort_limit = 100U;
	config->max_tuple_bytes = 512U;
	config->bucket_count = 131072U;
	config->max_build_rows = 100000U;
	config->session_id = (uint32_t)time(NULL) ^ UINT32_C(0xc5d10009);
	config->hash_seed = UINT32_C(2166136261);

	for(i = 2; i < argc; i++)
	{
		const char *name = argv[i];
		const char *value = NULL;

		if(strcmp(name, "--descending") == 0)
			config->sort_descending = 1U;
		else if(strcmp(name, "--nulls-first") == 0)
			config->nulls_first = 1U;
		else if(strcmp(name, "--trace-commands") == 0)
			config->trace_commands = 1U;
		else if(strcmp(name, "--device") == 0 &&
				option_value(argc, argv, &i, &config->device) == 0)
			continue;
		else if(strcmp(name, "--file") == 0 &&
				option_value(argc, argv, &i, &value) == 0 &&
				add_file(&config->files, value) == 0)
			continue;
		else if(strcmp(name, "--build-file") == 0 &&
				option_value(argc, argv, &i, &value) == 0 &&
				add_file(&config->build_files, value) == 0)
			continue;
		else if(strcmp(name, "--probe-file") == 0 &&
				option_value(argc, argv, &i, &value) == 0 &&
				add_file(&config->probe_files, value) == 0)
			continue;
		else if(strcmp(name, "--desc") == 0 &&
				option_value(argc, argv, &i, &config->desc_spec) == 0)
			continue;
		else if(strcmp(name, "--build-desc") == 0 &&
				option_value(argc, argv, &i, &config->build_desc_spec) == 0)
			continue;
		else if(strcmp(name, "--probe-desc") == 0 &&
				option_value(argc, argv, &i, &config->probe_desc_spec) == 0)
			continue;
		else if(strcmp(name, "--mode") == 0 &&
				option_value(argc, argv, &i, &config->mode) == 0)
			continue;
		else if(strcmp(name, "--label") == 0 &&
				option_value(argc, argv, &i, &config->label) == 0)
			continue;
		else if(strcmp(name, "--attrs") == 0 &&
				option_value(argc, argv, &i, &config->attrs) == 0)
			continue;
		else if(strcmp(name, "--ops") == 0 &&
				option_value(argc, argv, &i, &config->ops) == 0)
			continue;
		else if(strcmp(name, "--key-type") == 0 &&
				option_value(argc, argv, &i, &config->key_type) == 0)
			continue;
		else if((strcmp(name, "--key-attr") == 0 ||
				 strcmp(name, "--build-key") == 0 ||
				 strcmp(name, "--probe-key") == 0 ||
				 strcmp(name, "--split-lbas") == 0 ||
				 strcmp(name, "--result-pages") == 0 ||
				 strcmp(name, "--timeout-ms") == 0 ||
				 strcmp(name, "--limit") == 0 ||
				 strcmp(name, "--max-tuple-bytes") == 0 ||
				 strcmp(name, "--buckets") == 0 ||
				 strcmp(name, "--max-build-rows") == 0 ||
				 strcmp(name, "--session-id") == 0 ||
				 strcmp(name, "--hash-seed") == 0) &&
				 option_value(argc, argv, &i, &value) == 0)
		{
			uint32_t *target = NULL;
			if(strcmp(name, "--key-attr") == 0) target = &config->key_attr;
			else if(strcmp(name, "--build-key") == 0) target = &config->build_key_attr;
			else if(strcmp(name, "--probe-key") == 0) target = &config->probe_key_attr;
			else if(strcmp(name, "--split-lbas") == 0) target = &config->split_lbas;
			else if(strcmp(name, "--result-pages") == 0) target = &config->result_pages;
			else if(strcmp(name, "--timeout-ms") == 0) target = &config->timeout_ms;
			else if(strcmp(name, "--limit") == 0) target = &config->sort_limit;
			else if(strcmp(name, "--max-tuple-bytes") == 0) target = &config->max_tuple_bytes;
			else if(strcmp(name, "--buckets") == 0) target = &config->bucket_count;
			else if(strcmp(name, "--max-build-rows") == 0) target = &config->max_build_rows;
			else if(strcmp(name, "--session-id") == 0) target = &config->session_id;
			else if(strcmp(name, "--hash-seed") == 0) target = &config->hash_seed;
			if(!target || parse_u32(value, target) < 0)
				return -1;
			continue;
		}
		else
			return -1;
	}

	if(!config->device || !config->mode || config->split_lbas < 16U ||
	   config->result_pages == 0U || config->result_pages > 32U ||
	   config->timeout_ms == 0U || config->max_tuple_bytes < 23U ||
	   config->max_tuple_bytes > 8192U)
		return -1;
	if(config->label)
		for(const char *p = config->label; *p; p++)
			if(!( (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') ||
				  (*p >= '0' && *p <= '9') || *p == '_' || *p == '-' || *p == '.'))
				return -1;
	if(config->operation == BENCH_OPERATION_JOIN)
		return (config->build_files.count != 0U && config->probe_files.count != 0U &&
			config->build_desc_spec && config->probe_desc_spec && config->key_type &&
			config->bucket_count >= 64U &&
			(config->bucket_count & (config->bucket_count - 1U)) == 0U &&
			config->max_build_rows != 0U) ? 0 : -1;
	return (config->files.count != 0U && config->desc_spec &&
		((config->operation == BENCH_OPERATION_PROJECTION && config->attrs) ||
		 (config->operation == BENCH_OPERATION_AGGREGATE && config->ops) ||
		 (config->operation == BENCH_OPERATION_TOPK && config->key_type &&
		  config->sort_limit != 0U))) ? 0 : -1;
}

static uint8_t parse_value_type(const char *name)
{
	if(name && strcmp(name, "int4") == 0) return FILTER_VALUE_TYPE_INT32;
	if(name && strcmp(name, "int8") == 0) return FILTER_VALUE_TYPE_INT64;
	if(name && strcmp(name, "float8") == 0) return FILTER_VALUE_TYPE_FLOAT64;
	if(name && strcmp(name, "bytes") == 0) return FILTER_VALUE_TYPE_BYTES;
	return 0U;
}

static const char *value_type_name(uint8_t type)
{
	if(type == FILTER_VALUE_TYPE_INT32) return "int4";
	if(type == FILTER_VALUE_TYPE_INT64) return "int8";
	if(type == FILTER_VALUE_TYPE_FLOAT64) return "float8";
	if(type == FILTER_VALUE_TYPE_BYTES) return "bytes";
	return "unknown";
}

static uint8_t parse_projection_mode(const char *mode)
{
	if(mode && strcmp(mode, "arm") == 0) return FILTER_PROJECTION_MODE_ARM;
	if(mode && strcmp(mode, "fpga") == 0) return FILTER_PROJECTION_MODE_FPGA;
	return 0U;
}

static uint8_t parse_compute_mode(const char *mode)
{
	if(mode && strcmp(mode, "arm") == 0) return FILTER_COMPUTE_MODE_ARM;
	if(mode && strcmp(mode, "fpga") == 0) return FILTER_COMPUTE_MODE_FPGA;
	if(mode && strcmp(mode, "auto") == 0) return FILTER_COMPUTE_MODE_AUTO;
	return 0U;
}

static uint8_t parse_join_mode(const char *mode)
{
	if(mode && strcmp(mode, "arm") == 0) return FILTER_JOIN_MODE_ARM;
	if(mode && strcmp(mode, "hybrid") == 0) return FILTER_JOIN_MODE_HYBRID;
	if(mode && strcmp(mode, "auto") == 0) return FILTER_JOIN_MODE_AUTO;
	return 0U;
}

static int parse_index_list(const char *spec, uint8_t *values, uint8_t *count)
{
	char *copy;
	char *cursor;
	char *token;
	uint8_t used = 0U;
	uint32_t seen = 0U;

	if(!spec || !values || !count)
		return -1;
	copy = strdup(spec);
	if(!copy)
		return -1;
	cursor = copy;
	while((token = next_token(&cursor, ',')) != NULL)
	{
		uint32_t value;
		if(*token == '\0' || used >= FILTER_PROJECTION_MAX_COLUMNS ||
		   parse_u32(token, &value) < 0 || value >= CSD_OPERATOR_MAX_COLUMNS ||
		   (seen & (1U << value)) != 0U)
		{
			free(copy);
			return -1;
		}
		values[used++] = (uint8_t)value;
		seen |= 1U << value;
	}
	free(copy);
	*count = used;
	return used == 0U ? -1 : 0;
}

static int parse_aggregate_ops(const char *spec,
							   filter_compute_op_desc_t *ops,
							   uint8_t *op_count)
{
	char *copy;
	char *cursor;
	char *token;
	uint8_t used = 0U;

	if(!spec || !ops || !op_count)
		return -1;
	copy = strdup(spec);
	if(!copy)
		return -1;
	cursor = copy;
	while((token = next_token(&cursor, ',')) != NULL)
	{
		char *part = token;
		char *opcode_name = next_token(&part, ':');
		char *type_name = next_token(&part, ':');
		char *attr_name = next_token(&part, ':');
		uint32_t attr;
		uint8_t opcode = 0U;
		uint8_t type;

		if(opcode_name && strcmp(opcode_name, "sum") == 0) opcode = FILTER_COMPUTE_OP_SUM;
		else if(opcode_name && strcmp(opcode_name, "minimum") == 0) opcode = FILTER_COMPUTE_OP_MIN;
		else if(opcode_name && strcmp(opcode_name, "maximum") == 0) opcode = FILTER_COMPUTE_OP_MAX;
		type = parse_value_type(type_name);
		if(used >= FILTER_COMPUTE_MAX_OPS || opcode == 0U ||
		   type == 0U || type == FILTER_VALUE_TYPE_BYTES || !attr_name ||
		   part != NULL || parse_u32(attr_name, &attr) < 0 ||
		   attr >= CSD_OPERATOR_MAX_COLUMNS)
		{
			free(copy);
			return -1;
		}
		memset(&ops[used], 0, sizeof(ops[used]));
		ops[used].opcode = opcode;
		ops[used].value_type = type;
		ops[used].attr_index = (uint8_t)attr;
		used++;
	}
	free(copy);
	*op_count = used;
	return used == 0U ? -1 : 0;
}

static void *aligned_buffer_alloc(size_t bytes)
{
#ifdef _WIN32
	return _aligned_malloc(bytes, 4096U);
#else
	void *buffer = NULL;
	if(posix_memalign(&buffer, 4096U, bytes) != 0)
		return NULL;
	return buffer;
#endif
}

static void aligned_buffer_free(void *buffer)
{
#ifdef _WIN32
	_aligned_free(buffer);
#else
	free(buffer);
#endif
}

static bench_cpu_time_t cpu_time_now(void)
{
	bench_cpu_time_t value = {0.0, 0.0};
#if defined(__linux__) && !defined(CSD_OPERATOR_IO_MOCK)
	struct rusage usage;
	if(getrusage(RUSAGE_SELF, &usage) == 0)
	{
		value.user_ms = (double)usage.ru_utime.tv_sec * 1000.0 +
			(double)usage.ru_utime.tv_usec / 1000.0;
		value.system_ms = (double)usage.ru_stime.tv_sec * 1000.0 +
			(double)usage.ru_stime.tv_usec / 1000.0;
	}
#endif
	return value;
}

static uint32_t result_dma_pages(const uint8_t *result)
{
	uint32_t pages = 0U;
	memcpy(&pages, result, sizeof(pages));
	return pages;
}

static void record_effective_mode(bench_run_stats_t *stats, uint8_t mode,
								  uint8_t arm, uint8_t fpga, uint8_t mixed)
{
	if(mode == arm) stats->arm_commands++;
	else if(mode == fpga) stats->fpga_commands++;
	else if(mode == mixed) stats->mixed_commands++;
}

static void print_common_stats(const bench_config_t *config,
							   const bench_run_stats_t *stats)
{
	printf("\"engine\":\"csd\",\"label\":\"%s\",\"mode\":\"%s\",\"elapsed_ms\":%.3f,"
		"\"setup_ms\":%.3f,\"scan_execute_merge_ms\":%.3f,\"cpu_user_ms\":%.3f,"
		"\"cpu_system_ms\":%.3f,\"commands\":%" PRIu64 ","
		"\"input_lbas\":%" PRIu64 ",\"input_bytes\":%" PRIu64 ","
		"\"input_rows\":%" PRIu64 ",\"output_rows\":%" PRIu64 ","
		"\"result_bytes_used\":%" PRIu64 ",\"result_dma_bytes\":%" PRIu64 ","
		"\"arm_commands\":%" PRIu64 ",\"fpga_commands\":%" PRIu64 ","
		"\"mixed_commands\":%" PRIu64 ",\"fallback_commands\":%" PRIu64 ","
		"\"flags_or\":%u",
		config->label ? config->label : "", config->mode, stats->elapsed_ms,
		stats->setup_ms, stats->scan_execute_merge_ms,
		stats->cpu_user_ms, stats->cpu_system_ms,
		stats->commands, stats->input_lbas, stats->input_lbas * CSD_OPERATOR_LBA_BYTES,
		stats->input_rows, stats->output_rows, stats->result_bytes_used,
		stats->result_dma_bytes, stats->arm_commands, stats->fpga_commands,
		stats->mixed_commands, stats->fallback_commands, stats->flags_or);
}

static int prepare_single_relation(const bench_config_t *config,
								   csd_operator_tuple_desc_t *desc,
								   uint32_t *natts,
								   csd_operator_extent_list_t *extents,
								   char *error)
{
	if(csd_operator_parse_tuple_desc(config->desc_spec, desc, natts) < 0)
	{
		snprintf(error, BENCH_ERROR_BYTES, "invalid tuple descriptor: %s", config->desc_spec);
		return -1;
	}
	if(csd_operator_collect_extents(config->device, config->files.items,
			config->files.count, config->split_lbas, extents,
			error, BENCH_ERROR_BYTES) < 0)
		return -1;
	return 0;
}

static int run_projection(const bench_config_t *config)
{
	csd_operator_tuple_desc_t desc;
	csd_operator_extent_list_t extents = {0};
	uint8_t attrs[FILTER_PROJECTION_MAX_COLUMNS];
	uint8_t attr_count = 0U;
	uint8_t mode = parse_projection_mode(config->mode);
	uint32_t natts = 0U;
	size_t result_bytes = (size_t)config->result_pages * CSD_OPERATOR_PAGE_BYTES;
	uint8_t *metadata = NULL;
	uint8_t *result = NULL;
	bench_run_stats_t stats = {0};
	bench_cpu_time_t cpu_start;
	bench_cpu_time_t cpu_end;
	uint64_t checksum = csd_canonical_checksum_begin();
	char error[BENCH_ERROR_BYTES] = {0};
	double total_start_ms;
	double execute_start_ms;
	double end_ms;
	int fd = -1;
	int status = 1;

	cpu_start = cpu_time_now();
	total_start_ms = csd_operator_monotonic_ms();
	if(mode == 0U || parse_index_list(config->attrs, attrs, &attr_count) < 0 ||
	   prepare_single_relation(config, &desc, &natts, &extents, error) < 0)
		goto fail;
	for(uint32_t i = 0U; i < attr_count; i++)
		if(attrs[i] >= natts)
		{
			snprintf(error, sizeof(error), "projection attribute %u is outside natts=%u", attrs[i], natts);
			goto fail;
		}
	metadata = (uint8_t *)aligned_buffer_alloc(CSD_OPERATOR_METADATA_BYTES);
	result = (uint8_t *)aligned_buffer_alloc(result_bytes);
	if(!metadata || !result)
	{
		snprintf(error, sizeof(error), "aligned buffer allocation failed");
		goto fail;
	}
	fd = csd_operator_open_device(config->device, error, sizeof(error));
	if(fd < 0)
		goto fail;

	execute_start_ms = csd_operator_monotonic_ms();
	for(size_t i = 0U; i < extents.count; i++)
	{
		filter_projection_result_view_t view;
		const filter_projection_row_header_t *row;
		const filter_projection_column_t *columns;
		uint64_t command_checksum = 0U;
		uint64_t column_checksum[FILTER_PROJECTION_MAX_COLUMNS];
		uint64_t unordered_row_xor = 0U;
		uint64_t unordered_row_sum = 0U;
		int rc;

		if(config->trace_commands)
		{
			command_checksum = csd_canonical_checksum_begin();
			for(uint32_t column = 0U; column < attr_count; column++)
				column_checksum[column] = csd_canonical_checksum_begin();
		}

		csd_operator_prepare_metadata(metadata, &desc, natts, &extents.items[i]);
		if(filter_projection_build_metadata(metadata + 2U * 4096U, 4096U,
				mode, attrs, attr_count, config->max_tuple_bytes) < 0 ||
		   csd_operator_execute(fd, metadata, result, result_bytes,
				config->timeout_ms, error, sizeof(error)) < 0 ||
		   filter_projection_result_open(&view, result, result_bytes) < 0)
		{
			if(error[0] == '\0') snprintf(error, sizeof(error), "projection result decode failed at command %" PRIu64, (uint64_t)i);
			goto fail;
		}
		if(view.header->error_code != FILTER_PROJECTION_ERROR_NONE ||
		   (view.header->flags & (FILTER_PROJECTION_RESULT_FLAG_FULL |
			FILTER_PROJECTION_RESULT_FLAG_INVALID_STAGE |
			FILTER_PROJECTION_RESULT_FLAG_ARM_ERROR)) != 0U)
		{
			snprintf(error, sizeof(error), "projection command %" PRIu64 " failed: error=%u flags=0x%x",
				(uint64_t)i, view.header->error_code, view.header->flags);
			goto fail;
		}
		while((rc = filter_projection_result_next(&view, &row, &columns)) > 0)
		{
			uint64_t row_checksum = 0U;

			csd_canonical_checksum_row(&checksum);
			if(config->trace_commands)
			{
				row_checksum = csd_canonical_checksum_begin();
				csd_canonical_checksum_row(&command_checksum);
				csd_canonical_checksum_row(&row_checksum);
				for(uint32_t column = 0U; column < attr_count; column++)
					csd_canonical_checksum_row(&column_checksum[column]);
			}
			for(uint32_t column = 0U; column < attr_count; column++)
			{
				const void *data;
				uint16_t length;
				uint16_t flags;
				if(filter_projection_column_data(row, &columns[column], &data,
										 &length, &flags) < 0)
				{
					snprintf(error, sizeof(error), "malformed projected column at command %" PRIu64, (uint64_t)i);
					goto fail;
				}
				if(csd_canonical_checksum_raw_field(&checksum, flags, data, length) < 0)
				{
					snprintf(error, sizeof(error), "cannot canonicalize projected column at command %" PRIu64,
						(uint64_t)i);
					goto fail;
				}
				if(config->trace_commands &&
				   (csd_canonical_checksum_raw_field(&command_checksum,
						flags, data, length) < 0 ||
				    csd_canonical_checksum_raw_field(&row_checksum,
						flags, data, length) < 0 ||
				    csd_canonical_checksum_raw_field(&column_checksum[column],
						flags, data, length) < 0))
				{
					snprintf(error, sizeof(error), "cannot trace projected column at command %" PRIu64,
						(uint64_t)i);
					goto fail;
				}
			}
			if(config->trace_commands)
			{
				unordered_row_xor ^= row_checksum;
				unordered_row_sum += row_checksum;
			}
		}
		if(rc < 0)
		{
			snprintf(error, sizeof(error), "projection row stream is malformed at command %" PRIu64, (uint64_t)i);
			goto fail;
		}
		if(config->trace_commands)
		{
			fprintf(stderr,
				"projection_trace label=%s mode=%s command=%" PRIu64
				" start_lba=%" PRIu64 " lba_count=%" PRIu64
				" rows=%u bytes_used=%u checksum=%016" PRIx64
				" unordered_xor=%016" PRIx64 " unordered_sum=%016" PRIx64,
				config->label ? config->label : "", config->mode,
				(uint64_t)i, extents.items[i].start_lba,
				extents.items[i].lba_count, view.header->row_count,
				view.header->bytes_used, command_checksum,
				unordered_row_xor, unordered_row_sum);
			for(uint32_t column = 0U; column < attr_count; column++)
				fprintf(stderr, " col%u=%016" PRIx64, column,
					column_checksum[column]);
			fputc('\n', stderr);
		}
		stats.commands++;
		stats.input_lbas += extents.items[i].lba_count;
		stats.input_rows += view.header->row_count;
		stats.output_rows += view.header->row_count;
		stats.result_bytes_used += view.header->bytes_used;
		stats.result_dma_bytes += (uint64_t)result_dma_pages(result) * CSD_OPERATOR_PAGE_BYTES;
		stats.flags_or |= view.header->flags;
		if((view.header->flags & FILTER_PROJECTION_RESULT_FLAG_FPGA_FALLBACK) != 0U)
			stats.fallback_commands++;
		record_effective_mode(&stats, view.header->effective_mode,
			FILTER_PROJECTION_EFFECTIVE_ARM, FILTER_PROJECTION_EFFECTIVE_FPGA,
			FILTER_PROJECTION_EFFECTIVE_MIXED);
	}
	end_ms = csd_operator_monotonic_ms();
	stats.setup_ms = execute_start_ms - total_start_ms;
	stats.scan_execute_merge_ms = end_ms - execute_start_ms;
	stats.elapsed_ms = end_ms - total_start_ms;
	cpu_end = cpu_time_now();
	stats.cpu_user_ms = cpu_end.user_ms - cpu_start.user_ms;
	stats.cpu_system_ms = cpu_end.system_ms - cpu_start.system_ms;
	printf("{\"status\":\"ok\",\"operation\":\"projection\",");
	print_common_stats(config, &stats);
	printf(",\"column_count\":%u,\"checksum_fnv64\":\"%016" PRIx64 "\"}\n",
		attr_count, checksum);
	status = 0;
	goto done;

fail:
	fprintf(stderr, "projection failed: %s\n", error[0] ? error : "invalid arguments");
done:
	if(fd >= 0) close(fd);
	aligned_buffer_free(metadata);
	aligned_buffer_free(result);
	csd_operator_extent_list_free(&extents);
	return status;
}

static int float_compare_bits(uint64_t left_bits, uint64_t right_bits)
{
	double left;
	double right;
	memcpy(&left, &left_bits, sizeof(left));
	memcpy(&right, &right_bits, sizeof(right));
	if(isnan(left) || isnan(right))
	{
		if(isnan(left) && isnan(right)) return 0;
		return isnan(left) ? 1 : -1;
	}
	return (left > right) - (left < right);
}

static int raw_value_compare(uint8_t type,
						 const uint8_t *left,
						 uint32_t left_len,
						 const uint8_t *right,
						 uint32_t right_len)
{
	if(type == FILTER_VALUE_TYPE_INT32 && left_len == 4U && right_len == 4U)
	{
		int32_t a, b;
		memcpy(&a, left, 4U); memcpy(&b, right, 4U);
		return (a > b) - (a < b);
	}
	if(type == FILTER_VALUE_TYPE_INT64 && left_len == 8U && right_len == 8U)
	{
		int64_t a, b;
		memcpy(&a, left, 8U); memcpy(&b, right, 8U);
		return (a > b) - (a < b);
	}
	if(type == FILTER_VALUE_TYPE_FLOAT64 && left_len == 8U && right_len == 8U)
	{
		uint64_t a, b;
		memcpy(&a, left, 8U); memcpy(&b, right, 8U);
		return float_compare_bits(a, b);
	}
	{
		uint32_t common = left_len < right_len ? left_len : right_len;
		int compared = common ? memcmp(left, right, common) : 0;
		if(compared != 0) return compared < 0 ? -1 : 1;
		return (left_len > right_len) - (left_len < right_len);
	}
}

static int merge_aggregate_value(aggregate_state_t *state,
								 const filter_compute_aggregate_value_t *value)
{
	if(memcmp(&state->op, value, 4U) != 0 ||
	   (value->flags & ~(FILTER_COMPUTE_VALUE_FLAG_NULL |
		FILTER_COMPUTE_VALUE_FLAG_OVERFLOW | FILTER_COMPUTE_VALUE_FLAG_NAN)) != 0U)
		return -1;
	state->nonnull_count += value->nonnull_count;
	state->flags |= value->flags;
	if((value->flags & FILTER_COMPUTE_VALUE_FLAG_NULL) != 0U)
		return 0;
	if(state->op.opcode == FILTER_COMPUTE_OP_SUM)
	{
		if(state->op.value_type == FILTER_VALUE_TYPE_INT32 && value->value_bytes == 8U)
		{
			int64_t part;
			memcpy(&part, value->value, sizeof(part));
			state->int32_sum += part;
		}
		else if(state->op.value_type == FILTER_VALUE_TYPE_INT64 && value->value_bytes == 16U)
		{
			__int128 part;
			memcpy(&part, value->value, sizeof(part));
			state->int64_sum += part;
		}
		else if(state->op.value_type == FILTER_VALUE_TYPE_FLOAT64 && value->value_bytes == 8U)
		{
			double part;
			memcpy(&part, value->value, sizeof(part));
			state->float_sum += part;
		}
		else return -1;
		state->has_value = 1U;
		return 0;
	}
	{
		uint32_t expected = state->op.value_type == FILTER_VALUE_TYPE_INT32 ? 4U : 8U;
		int comparison;
		if(value->value_bytes != expected)
			return -1;
		if(!state->has_value)
		{
			memcpy(state->extremum, value->value, expected);
			state->has_value = 1U;
			return 0;
		}
		comparison = raw_value_compare(state->op.value_type, value->value, expected,
			state->extremum, expected);
		if((state->op.opcode == FILTER_COMPUTE_OP_MIN && comparison < 0) ||
		   (state->op.opcode == FILTER_COMPUTE_OP_MAX && comparison > 0))
			memcpy(state->extremum, value->value, expected);
	}
	return 0;
}

static void int128_to_text(__int128 value, char output[64])
{
	char reversed[64];
	size_t used = 0U;
	uint32_t negative = value < 0;
	unsigned __int128 magnitude = negative ?
		(unsigned __int128)(-(value + 1)) + 1U : (unsigned __int128)value;

	do
	{
		reversed[used++] = (char)('0' + magnitude % 10U);
		magnitude /= 10U;
	} while(magnitude != 0U);
	if(negative) reversed[used++] = '-';
	for(size_t i = 0U; i < used; i++) output[i] = reversed[used - i - 1U];
	output[used] = '\0';
}

static const char *aggregate_op_name(uint8_t opcode)
{
	if(opcode == FILTER_COMPUTE_OP_SUM) return "sum";
	if(opcode == FILTER_COMPUTE_OP_MIN) return "minimum";
	if(opcode == FILTER_COMPUTE_OP_MAX) return "maximum";
	return "unknown";
}

static void print_aggregate_value(const aggregate_state_t *state)
{
	char value_text[128] = "null";
	if(state->has_value)
	{
		if(state->op.opcode == FILTER_COMPUTE_OP_SUM)
		{
			if(state->op.value_type == FILTER_VALUE_TYPE_INT32)
				snprintf(value_text, sizeof(value_text), "%" PRId64, state->int32_sum);
			else if(state->op.value_type == FILTER_VALUE_TYPE_INT64)
				int128_to_text(state->int64_sum, value_text);
			else
				snprintf(value_text, sizeof(value_text), "%.17g", state->float_sum);
		}
		else if(state->op.value_type == FILTER_VALUE_TYPE_INT32)
		{
			int32_t value;
			memcpy(&value, state->extremum, sizeof(value));
			snprintf(value_text, sizeof(value_text), "%" PRId32, value);
		}
		else if(state->op.value_type == FILTER_VALUE_TYPE_INT64)
		{
			int64_t value;
			memcpy(&value, state->extremum, sizeof(value));
			snprintf(value_text, sizeof(value_text), "%" PRId64, value);
		}
		else
		{
			double value;
			memcpy(&value, state->extremum, sizeof(value));
			snprintf(value_text, sizeof(value_text), "%.17g", value);
		}
	}
	printf("{\"op\":\"%s\",\"type\":\"%s\",\"attr\":%u,"
		"\"nonnull\":%" PRIu64 ",\"flags\":%u,\"value\":\"%s\"}",
		aggregate_op_name(state->op.opcode), value_type_name(state->op.value_type),
		state->op.attr_index, state->nonnull_count, state->flags, value_text);
}

static int run_aggregate(const bench_config_t *config)
{
	csd_operator_tuple_desc_t desc;
	csd_operator_extent_list_t extents = {0};
	filter_compute_op_desc_t ops[FILTER_COMPUTE_MAX_OPS];
	aggregate_state_t merged[FILTER_COMPUTE_MAX_OPS];
	uint8_t op_count = 0U;
	uint8_t mode = parse_compute_mode(config->mode);
	uint32_t natts = 0U;
	size_t result_bytes = (size_t)config->result_pages * CSD_OPERATOR_PAGE_BYTES;
	uint8_t *metadata = NULL;
	uint8_t *result = NULL;
	bench_run_stats_t stats = {0};
	bench_cpu_time_t cpu_start;
	bench_cpu_time_t cpu_end;
	char error[BENCH_ERROR_BYTES] = {0};
	double total_start_ms;
	double execute_start_ms;
	double end_ms;
	int fd = -1;
	int status = 1;

	cpu_start = cpu_time_now();
	total_start_ms = csd_operator_monotonic_ms();
	memset(ops, 0, sizeof(ops));
	memset(merged, 0, sizeof(merged));
	if(mode == 0U || parse_aggregate_ops(config->ops, ops, &op_count) < 0 ||
	   prepare_single_relation(config, &desc, &natts, &extents, error) < 0)
		goto fail;
	for(uint32_t i = 0U; i < op_count; i++)
	{
		if(ops[i].attr_index >= natts)
		{
			snprintf(error, sizeof(error), "aggregate attribute %u is outside natts=%u", ops[i].attr_index, natts);
			goto fail;
		}
		merged[i].op = ops[i];
	}
	metadata = (uint8_t *)aligned_buffer_alloc(CSD_OPERATOR_METADATA_BYTES);
	result = (uint8_t *)aligned_buffer_alloc(result_bytes);
	if(!metadata || !result)
	{
		snprintf(error, sizeof(error), "aligned buffer allocation failed");
		goto fail;
	}
	fd = csd_operator_open_device(config->device, error, sizeof(error));
	if(fd < 0) goto fail;

	execute_start_ms = csd_operator_monotonic_ms();
	for(size_t i = 0U; i < extents.count; i++)
	{
		filter_compute_result_view_t view;
		const filter_compute_aggregate_value_t *values;
		uint32_t value_count;

		csd_operator_prepare_metadata(metadata, &desc, natts, &extents.items[i]);
		if(filter_compute_build_aggregate_metadata(metadata + 2U * 4096U, 4096U,
				mode, ops, op_count) < 0 ||
		   csd_operator_execute(fd, metadata, result, result_bytes,
				config->timeout_ms, error, sizeof(error)) < 0 ||
		   filter_compute_result_open(&view, result, result_bytes) < 0 ||
		   filter_compute_aggregate_values(&view, &values, &value_count) < 0)
		{
			if(error[0] == '\0') snprintf(error, sizeof(error), "aggregate result decode failed at command %" PRIu64, (uint64_t)i);
			goto fail;
		}
		if(view.header->error_code != FILTER_COMPUTE_ERROR_NONE ||
		   (view.header->flags & ~(FILTER_COMPUTE_RESULT_FLAG_FPGA_FALLBACK |
			FILTER_COMPUTE_RESULT_FLAG_NAN)) != 0U || value_count != op_count)
		{
			snprintf(error, sizeof(error), "aggregate command %" PRIu64 " failed: error=%u flags=0x%x values=%u",
				(uint64_t)i, view.header->error_code, view.header->flags, value_count);
			goto fail;
		}
		for(uint32_t op = 0U; op < op_count; op++)
			if(merge_aggregate_value(&merged[op], &values[op]) < 0)
			{
				snprintf(error, sizeof(error), "aggregate value mismatch at command %" PRIu64 " op=%u", (uint64_t)i, op);
				goto fail;
			}
		stats.commands++;
		stats.input_lbas += extents.items[i].lba_count;
		stats.input_rows += view.header->input_rows;
		stats.output_rows += view.header->output_rows;
		stats.result_bytes_used += view.header->bytes_used;
		stats.result_dma_bytes += (uint64_t)result_dma_pages(result) * CSD_OPERATOR_PAGE_BYTES;
		stats.flags_or |= view.header->flags;
		if((view.header->flags & FILTER_COMPUTE_RESULT_FLAG_FPGA_FALLBACK) != 0U)
			stats.fallback_commands++;
		record_effective_mode(&stats, view.header->effective_mode,
			FILTER_COMPUTE_EFFECTIVE_ARM, FILTER_COMPUTE_EFFECTIVE_FPGA,
			FILTER_COMPUTE_EFFECTIVE_MIXED);
	}
	end_ms = csd_operator_monotonic_ms();
	stats.setup_ms = execute_start_ms - total_start_ms;
	stats.scan_execute_merge_ms = end_ms - execute_start_ms;
	stats.elapsed_ms = end_ms - total_start_ms;
	cpu_end = cpu_time_now();
	stats.cpu_user_ms = cpu_end.user_ms - cpu_start.user_ms;
	stats.cpu_system_ms = cpu_end.system_ms - cpu_start.system_ms;
	stats.output_rows = op_count;
	printf("{\"status\":\"ok\",\"operation\":\"aggregate\",");
	print_common_stats(config, &stats);
	printf(",\"values\":[");
	for(uint32_t i = 0U; i < op_count; i++)
	{
		if(i) putchar(',');
		print_aggregate_value(&merged[i]);
	}
	puts("]}");
	status = 0;
	goto done;

fail:
	fprintf(stderr, "aggregate failed: %s\n", error[0] ? error : "invalid arguments");
done:
	if(fd >= 0) close(fd);
	aligned_buffer_free(metadata);
	aligned_buffer_free(result);
	csd_operator_extent_list_free(&extents);
	return status;
}

static int top_key_push(top_key_list_t *list,
						const void *data,
						uint32_t length,
						uint32_t is_null,
						uint64_t sequence)
{
	top_key_t *item;
	if(list->count >= BENCH_MAX_TOP_KEYS)
		return -1;
	if(list->count == list->capacity)
	{
		size_t capacity = list->capacity ? list->capacity * 2U : 256U;
		top_key_t *items;
		if(capacity > BENCH_MAX_TOP_KEYS) capacity = BENCH_MAX_TOP_KEYS;
		items = (top_key_t *)realloc(list->items, capacity * sizeof(*items));
		if(!items) return -1;
		list->items = items;
		list->capacity = capacity;
	}
	item = &list->items[list->count];
	memset(item, 0, sizeof(*item));
	item->is_null = is_null;
	item->length = length;
	item->sequence = sequence;
	if(!is_null && length != 0U)
	{
		item->data = (uint8_t *)malloc(length);
		if(!item->data) return -1;
		memcpy(item->data, data, length);
	}
	list->count++;
	return 0;
}

static int top_key_compare(const void *left_ptr, const void *right_ptr)
{
	const top_key_t *left = (const top_key_t *)left_ptr;
	const top_key_t *right = (const top_key_t *)right_ptr;
	int compared;
	if(left->is_null || right->is_null)
	{
		if(left->is_null && right->is_null) compared = 0;
		else if((top_compare_flags & FILTER_COMPUTE_OP_FLAG_NULLS_FIRST) != 0U)
			compared = left->is_null ? -1 : 1;
		else compared = left->is_null ? 1 : -1;
	}
	else
	{
		compared = raw_value_compare(top_compare_type, left->data, left->length,
			right->data, right->length);
		if((top_compare_flags & FILTER_COMPUTE_OP_FLAG_DESC) != 0U)
			compared = -compared;
	}
	if(compared == 0)
		compared = (left->sequence > right->sequence) - (left->sequence < right->sequence);
	return compared;
}

static void top_key_list_free(top_key_list_t *list)
{
	if(!list) return;
	for(size_t i = 0U; i < list->count; i++) free(list->items[i].data);
	free(list->items);
	memset(list, 0, sizeof(*list));
}

static void top_key_to_text(const top_key_t *key, uint8_t type, char output[128])
{
	if(!key || key->is_null)
	{
		strcpy(output, "null");
		return;
	}
	if(type == FILTER_VALUE_TYPE_INT32 && key->length == 4U)
	{
		int32_t value;
		memcpy(&value, key->data, sizeof(value));
		snprintf(output, 128U, "%" PRId32, value);
	}
	else if(type == FILTER_VALUE_TYPE_INT64 && key->length == 8U)
	{
		int64_t value;
		memcpy(&value, key->data, sizeof(value));
		snprintf(output, 128U, "%" PRId64, value);
	}
	else if(type == FILTER_VALUE_TYPE_FLOAT64 && key->length == 8U)
	{
		double value;
		memcpy(&value, key->data, sizeof(value));
		snprintf(output, 128U, "%.17g", value);
	}
	else
	{
		size_t used = 0U;
		for(uint32_t i = 0U; i < key->length && i < 32U && used + 2U < 128U; i++)
			used += (size_t)snprintf(output + used, 128U - used, "%02x", key->data[i]);
		output[used] = '\0';
	}
}

static int run_topk(const bench_config_t *config)
{
	csd_operator_tuple_desc_t desc;
	csd_operator_extent_list_t extents = {0};
	top_key_list_t keys = {0};
	uint8_t mode = parse_compute_mode(config->mode);
	uint8_t type = parse_value_type(config->key_type);
	uint8_t sort_flags = (config->sort_descending ? FILTER_COMPUTE_OP_FLAG_DESC : 0U) |
		(config->nulls_first ? FILTER_COMPUTE_OP_FLAG_NULLS_FIRST : 0U);
	uint32_t natts = 0U;
	size_t result_bytes = (size_t)config->result_pages * CSD_OPERATOR_PAGE_BYTES;
	uint8_t *metadata = NULL;
	uint8_t *result = NULL;
	bench_run_stats_t stats = {0};
	bench_cpu_time_t cpu_start;
	bench_cpu_time_t cpu_end;
	char error[BENCH_ERROR_BYTES] = {0};
	uint64_t sequence = 0U;
	uint64_t checksum = csd_canonical_checksum_begin();
	char first_key[128] = "null";
	char last_key[128] = "null";
	double total_start_ms;
	double execute_start_ms;
	double end_ms;
	int fd = -1;
	int status = 1;

	cpu_start = cpu_time_now();
	total_start_ms = csd_operator_monotonic_ms();
	if(mode == 0U || type == 0U || config->key_attr >= CSD_OPERATOR_MAX_COLUMNS ||
	   prepare_single_relation(config, &desc, &natts, &extents, error) < 0 ||
	   config->key_attr >= natts)
		goto fail;
	metadata = (uint8_t *)aligned_buffer_alloc(CSD_OPERATOR_METADATA_BYTES);
	result = (uint8_t *)aligned_buffer_alloc(result_bytes);
	if(!metadata || !result)
	{
		snprintf(error, sizeof(error), "aligned buffer allocation failed");
		goto fail;
	}
	fd = csd_operator_open_device(config->device, error, sizeof(error));
	if(fd < 0) goto fail;

	execute_start_ms = csd_operator_monotonic_ms();
	for(size_t i = 0U; i < extents.count; i++)
	{
		filter_compute_result_view_t view;
		const filter_compute_sort_row_t *row;
		const void *tuple;
		const void *key;
		int rc;

		csd_operator_prepare_metadata(metadata, &desc, natts, &extents.items[i]);
		if(filter_compute_build_sort_metadata(metadata + 2U * 4096U, 4096U,
				mode, (uint8_t)config->key_attr, type, sort_flags,
				config->sort_limit, config->max_tuple_bytes) < 0 ||
		   csd_operator_execute(fd, metadata, result, result_bytes,
				config->timeout_ms, error, sizeof(error)) < 0 ||
		   filter_compute_result_open(&view, result, result_bytes) < 0)
		{
			if(error[0] == '\0') snprintf(error, sizeof(error), "Top-K result decode failed at command %" PRIu64, (uint64_t)i);
			goto fail;
		}
		if(view.header->error_code != FILTER_COMPUTE_ERROR_NONE ||
		   (view.header->flags & ~(FILTER_COMPUTE_RESULT_FLAG_TRUNCATED |
			FILTER_COMPUTE_RESULT_FLAG_FPGA_FALLBACK)) != 0U)
		{
			snprintf(error, sizeof(error), "Top-K command %" PRIu64 " failed: error=%u flags=0x%x",
				(uint64_t)i, view.header->error_code, view.header->flags);
			goto fail;
		}
		while((rc = filter_compute_sort_next(&view, &row, &tuple, &key)) > 0)
		{
			(void)tuple;
			if(top_key_push(&keys, key, row->key_bytes,
				(row->flags & FILTER_COMPUTE_SORT_ROW_FLAG_KEY_NULL) != 0U,
				sequence++) < 0)
			{
				snprintf(error, sizeof(error), "too many Top-K candidates or out of memory");
				goto fail;
			}
		}
		if(rc < 0)
		{
			snprintf(error, sizeof(error), "Top-K row stream is malformed at command %" PRIu64, (uint64_t)i);
			goto fail;
		}
		stats.commands++;
		stats.input_lbas += extents.items[i].lba_count;
		stats.input_rows += view.header->input_rows;
		stats.result_bytes_used += view.header->bytes_used;
		stats.result_dma_bytes += (uint64_t)result_dma_pages(result) * CSD_OPERATOR_PAGE_BYTES;
		stats.flags_or |= view.header->flags;
		if((view.header->flags & FILTER_COMPUTE_RESULT_FLAG_FPGA_FALLBACK) != 0U)
			stats.fallback_commands++;
		record_effective_mode(&stats, view.header->effective_mode,
			FILTER_COMPUTE_EFFECTIVE_ARM, FILTER_COMPUTE_EFFECTIVE_FPGA,
			FILTER_COMPUTE_EFFECTIVE_MIXED);
	}
	top_compare_type = type;
	top_compare_flags = sort_flags;
	qsort(keys.items, keys.count, sizeof(keys.items[0]), top_key_compare);
	stats.output_rows = keys.count < config->sort_limit ? keys.count : config->sort_limit;
	for(size_t i = 0U; i < stats.output_rows; i++)
	{
		uint16_t flags = keys.items[i].is_null ? FILTER_PROJECTION_COLUMN_FLAG_NULL : 0U;
		uint16_t length = (uint16_t)keys.items[i].length;
		csd_canonical_checksum_row(&checksum);
		if(csd_canonical_checksum_raw_field(&checksum, flags,
			keys.items[i].data, length) < 0)
		{
			snprintf(error, sizeof(error), "cannot canonicalize Top-K key at row %" PRIu64,
				(uint64_t)i);
			goto fail;
		}
	}
	if(stats.output_rows != 0U)
	{
		top_key_to_text(&keys.items[0], type, first_key);
		top_key_to_text(&keys.items[stats.output_rows - 1U], type, last_key);
	}
	end_ms = csd_operator_monotonic_ms();
	stats.setup_ms = execute_start_ms - total_start_ms;
	stats.scan_execute_merge_ms = end_ms - execute_start_ms;
	stats.elapsed_ms = end_ms - total_start_ms;
	cpu_end = cpu_time_now();
	stats.cpu_user_ms = cpu_end.user_ms - cpu_start.user_ms;
	stats.cpu_system_ms = cpu_end.system_ms - cpu_start.system_ms;
	printf("{\"status\":\"ok\",\"operation\":\"topk\",");
	print_common_stats(config, &stats);
	printf(",\"limit\":%u,\"candidate_rows\":%" PRIu64 ",\"key_type\":\"%s\","
		"\"first_key\":\"%s\",\"last_key\":\"%s\","
		"\"key_checksum_fnv64\":\"%016" PRIx64 "\"}\n",
		config->sort_limit, (uint64_t)keys.count, value_type_name(type),
		first_key, last_key, checksum);
	status = 0;
	goto done;

fail:
	fprintf(stderr, "topk failed: %s\n", error[0] ? error : "invalid arguments");
done:
	if(fd >= 0) close(fd);
	aligned_buffer_free(metadata);
	aligned_buffer_free(result);
	top_key_list_free(&keys);
	csd_operator_extent_list_free(&extents);
	return status;
}

static int run_join(const bench_config_t *config)
{
	csd_operator_tuple_desc_t build_desc;
	csd_operator_tuple_desc_t probe_desc;
	csd_operator_extent_list_t build_extents = {0};
	csd_operator_extent_list_t probe_extents = {0};
	filter_join_desc_t join;
	uint8_t mode = parse_join_mode(config->mode);
	uint8_t type = parse_value_type(config->key_type);
	uint32_t build_natts = 0U;
	uint32_t probe_natts = 0U;
	size_t result_bytes = (size_t)config->result_pages * CSD_OPERATOR_PAGE_BYTES;
	uint8_t *metadata = NULL;
	uint8_t *result = NULL;
	bench_run_stats_t stats = {0};
	bench_cpu_time_t cpu_start;
	bench_cpu_time_t cpu_end;
	char error[BENCH_ERROR_BYTES] = {0};
	uint64_t build_rows = 0U;
	uint64_t probe_rows = 0U;
	uint64_t match_rows = 0U;
	uint64_t collisions = 0U;
	uint32_t max_chain = 0U;
	uint64_t workspace_used = 0U;
	double total_start_ms;
	double execute_start_ms;
	double end_ms;
	int fd = -1;
	int status = 1;

	cpu_start = cpu_time_now();
	total_start_ms = csd_operator_monotonic_ms();
	if(mode == 0U || (type != FILTER_VALUE_TYPE_INT32 &&
		type != FILTER_VALUE_TYPE_INT64 && type != FILTER_VALUE_TYPE_FLOAT64 &&
		type != FILTER_VALUE_TYPE_BYTES) ||
	   csd_operator_parse_tuple_desc(config->build_desc_spec, &build_desc, &build_natts) < 0 ||
	   csd_operator_parse_tuple_desc(config->probe_desc_spec, &probe_desc, &probe_natts) < 0 ||
	   config->build_key_attr >= build_natts || config->probe_key_attr >= probe_natts ||
	   csd_operator_collect_extents(config->device, config->build_files.items,
		config->build_files.count, config->split_lbas, &build_extents,
		error, sizeof(error)) < 0 ||
	   csd_operator_collect_extents(config->device, config->probe_files.items,
		config->probe_files.count, config->split_lbas, &probe_extents,
		error, sizeof(error)) < 0)
		goto fail;

	metadata = (uint8_t *)aligned_buffer_alloc(CSD_OPERATOR_METADATA_BYTES);
	result = (uint8_t *)aligned_buffer_alloc(result_bytes);
	if(!metadata || !result)
	{
		snprintf(error, sizeof(error), "aligned buffer allocation failed");
		goto fail;
	}
	fd = csd_operator_open_device(config->device, error, sizeof(error));
	if(fd < 0) goto fail;

	memset(&join, 0, sizeof(join));
	join.mode = mode;
	join.key_count = 1U;
	join.session_id = config->session_id;
	join.bucket_count = config->bucket_count;
	join.max_build_rows = config->max_build_rows;
	join.max_output_rows = 0U;
	join.max_tuple_bytes = config->max_tuple_bytes;
	join.hash_seed = config->hash_seed;
	join.build_natts = (uint16_t)build_natts;
	join.probe_natts = (uint16_t)probe_natts;
	join.build_key_attr[0] = (uint8_t)config->build_key_attr;
	join.probe_key_attr[0] = (uint8_t)config->probe_key_attr;
	join.key_type[0] = type;

	execute_start_ms = csd_operator_monotonic_ms();
	for(size_t i = 0U; i < build_extents.count; i++)
	{
		filter_join_result_view_t view;
		join.phase = FILTER_JOIN_PHASE_BUILD;
		join.flags = FILTER_JOIN_DESC_FLAG_COUNT_ONLY |
			(i == 0U ? FILTER_JOIN_DESC_FLAG_RESET_ON_BUILD : 0U);
		csd_operator_prepare_metadata(metadata, &build_desc, build_natts,
			&build_extents.items[i]);
		if(filter_join_build_metadata(metadata + 2U * 4096U, 4096U, &join) < 0 ||
		   csd_operator_execute(fd, metadata, result, result_bytes,
				config->timeout_ms, error, sizeof(error)) < 0 ||
		   filter_join_result_open(&view, result, result_bytes) < 0)
		{
			if(error[0] == '\0') snprintf(error, sizeof(error), "Join BUILD decode failed at command %" PRIu64, (uint64_t)i);
			goto fail;
		}
		if(view.header->error_code != FILTER_JOIN_ERROR_NONE || view.header->flags != 0U)
		{
			snprintf(error, sizeof(error), "Join BUILD command %" PRIu64 " failed: error=%u flags=0x%x",
				(uint64_t)i, view.header->error_code, view.header->flags);
			goto fail;
		}
		build_rows = view.header->build_rows;
		workspace_used = view.header->workspace_used;
		collisions += view.header->hash_collisions;
		if(view.header->max_chain > max_chain) max_chain = view.header->max_chain;
		stats.commands++;
		stats.input_lbas += build_extents.items[i].lba_count;
		stats.result_bytes_used += view.header->bytes_used;
		stats.result_dma_bytes += (uint64_t)result_dma_pages(result) * CSD_OPERATOR_PAGE_BYTES;
		record_effective_mode(&stats, view.header->effective_mode,
			FILTER_JOIN_EFFECTIVE_ARM, FILTER_JOIN_EFFECTIVE_HYBRID,
			FILTER_JOIN_EFFECTIVE_MIXED);
	}
	for(size_t i = 0U; i < probe_extents.count; i++)
	{
		filter_join_result_view_t view;
		join.phase = FILTER_JOIN_PHASE_PROBE;
		join.flags = FILTER_JOIN_DESC_FLAG_COUNT_ONLY;
		csd_operator_prepare_metadata(metadata, &probe_desc, probe_natts,
			&probe_extents.items[i]);
		if(filter_join_build_metadata(metadata + 2U * 4096U, 4096U, &join) < 0 ||
		   csd_operator_execute(fd, metadata, result, result_bytes,
				config->timeout_ms, error, sizeof(error)) < 0 ||
		   filter_join_result_open(&view, result, result_bytes) < 0)
		{
			if(error[0] == '\0') snprintf(error, sizeof(error), "Join PROBE decode failed at command %" PRIu64, (uint64_t)i);
			goto fail;
		}
		if(view.header->error_code != FILTER_JOIN_ERROR_NONE || view.header->flags != 0U)
		{
			snprintf(error, sizeof(error), "Join PROBE command %" PRIu64 " failed: error=%u flags=0x%x",
				(uint64_t)i, view.header->error_code, view.header->flags);
			goto fail;
		}
		probe_rows += view.header->probe_rows;
		match_rows += view.header->match_rows;
		collisions += view.header->hash_collisions;
		if(view.header->max_chain > max_chain) max_chain = view.header->max_chain;
		workspace_used = view.header->workspace_used;
		stats.commands++;
		stats.input_lbas += probe_extents.items[i].lba_count;
		stats.result_bytes_used += view.header->bytes_used;
		stats.result_dma_bytes += (uint64_t)result_dma_pages(result) * CSD_OPERATOR_PAGE_BYTES;
		stats.flags_or |= view.header->flags;
		if((view.header->flags & FILTER_JOIN_RESULT_FLAG_FPGA_FALLBACK) != 0U)
			stats.fallback_commands++;
		record_effective_mode(&stats, view.header->effective_mode,
			FILTER_JOIN_EFFECTIVE_ARM, FILTER_JOIN_EFFECTIVE_HYBRID,
			FILTER_JOIN_EFFECTIVE_MIXED);
	}
	end_ms = csd_operator_monotonic_ms();
	stats.setup_ms = execute_start_ms - total_start_ms;
	stats.scan_execute_merge_ms = end_ms - execute_start_ms;
	stats.elapsed_ms = end_ms - total_start_ms;
	cpu_end = cpu_time_now();
	stats.cpu_user_ms = cpu_end.user_ms - cpu_start.user_ms;
	stats.cpu_system_ms = cpu_end.system_ms - cpu_start.system_ms;
	stats.input_rows = build_rows + probe_rows;
	stats.output_rows = match_rows;
	printf("{\"status\":\"ok\",\"operation\":\"join\",");
	print_common_stats(config, &stats);
	printf(",\"build_rows\":%" PRIu64 ",\"probe_rows\":%" PRIu64 ","
		"\"match_rows\":%" PRIu64 ",\"hash_collisions\":%" PRIu64 ","
		"\"max_chain\":%u,\"workspace_used\":%" PRIu64 ","
		"\"session_id\":%u,\"bucket_count\":%u}\n",
		build_rows, probe_rows, match_rows, collisions, max_chain, workspace_used,
		config->session_id, config->bucket_count);
	status = 0;
	goto done;

fail:
	fprintf(stderr, "join failed: %s\n", error[0] ? error : "invalid arguments");
done:
	if(fd >= 0) close(fd);
	aligned_buffer_free(metadata);
	aligned_buffer_free(result);
	csd_operator_extent_list_free(&build_extents);
	csd_operator_extent_list_free(&probe_extents);
	return status;
}

int main(int argc, char **argv)
{
	bench_config_t config;
	if(parse_arguments(argc, argv, &config) < 0)
	{
		usage(stderr);
		return 2;
	}
	if(config.operation == BENCH_OPERATION_PROJECTION) return run_projection(&config);
	if(config.operation == BENCH_OPERATION_AGGREGATE) return run_aggregate(&config);
	if(config.operation == BENCH_OPERATION_TOPK) return run_topk(&config);
	if(config.operation == BENCH_OPERATION_JOIN) return run_join(&config);
	return 2;
}
