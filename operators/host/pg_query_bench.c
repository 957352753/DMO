#define _GNU_SOURCE

#include "csd_canonical_checksum.h"

#include <libpq-fe.h>

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

#define PG_BENCH_FNV_OFFSET UINT64_C(1469598103934665603)
#define PG_BENCH_FNV_PRIME UINT64_C(1099511628211)

typedef struct process_cpu_ticks
{
	uint64_t user;
	uint64_t system;
} process_cpu_ticks_t;

typedef struct client_cpu_ms
{
	double user;
	double system;
} client_cpu_ms_t;

typedef struct pg_bench_config
{
	const char *conninfo;
	const char *sql;
	const char *label;
	uint32_t warmup;
	uint32_t repetitions;
	uint32_t binary;
} pg_bench_config_t;

static void usage(void)
{
	fputs("Usage: pg_query_bench --conninfo STRING --sql SQL --label NAME "
		"[--warmup N] [--repetitions N] [--binary 0|1]\n", stderr);
}

static int parse_u32(const char *text, uint32_t *value)
{
	char *end = NULL;
	unsigned long parsed;
	errno = 0;
	parsed = strtoul(text, &end, 10);
	if(errno != 0 || end == text || *end != '\0' || parsed > UINT32_MAX)
		return -1;
	*value = (uint32_t)parsed;
	return 0;
}

static int parse_args(int argc, char **argv, pg_bench_config_t *config)
{
	memset(config, 0, sizeof(*config));
	config->warmup = 1U;
	config->repetitions = 3U;
	config->binary = 1U;
	for(int i = 1; i < argc; i++)
	{
		const char *name = argv[i];
		const char *value;
		if(i + 1 >= argc)
			return -1;
		value = argv[++i];
		if(strcmp(name, "--conninfo") == 0) config->conninfo = value;
		else if(strcmp(name, "--sql") == 0) config->sql = value;
		else if(strcmp(name, "--label") == 0) config->label = value;
		else if(strcmp(name, "--warmup") == 0 && parse_u32(value, &config->warmup) == 0) continue;
		else if(strcmp(name, "--repetitions") == 0 && parse_u32(value, &config->repetitions) == 0) continue;
		else if(strcmp(name, "--binary") == 0 && parse_u32(value, &config->binary) == 0 && config->binary <= 1U) continue;
		else return -1;
	}
	return config->conninfo && config->sql && config->label && config->repetitions ? 0 : -1;
}

static double monotonic_ms(void)
{
	struct timespec now;
	if(clock_gettime(CLOCK_MONOTONIC, &now) != 0)
		return 0.0;
	return (double)now.tv_sec * 1000.0 + (double)now.tv_nsec / 1000000.0;
}

static client_cpu_ms_t client_cpu_now(void)
{
	client_cpu_ms_t value = {0.0, 0.0};
	struct rusage usage;
	if(getrusage(RUSAGE_SELF, &usage) == 0)
	{
		value.user = (double)usage.ru_utime.tv_sec * 1000.0 +
			(double)usage.ru_utime.tv_usec / 1000.0;
		value.system = (double)usage.ru_stime.tv_sec * 1000.0 +
			(double)usage.ru_stime.tv_usec / 1000.0;
	}
	return value;
}

static int read_process_cpu(pid_t pid, process_cpu_ticks_t *ticks)
{
	char path[64];
	char line[4096];
	char *cursor;
	char *end_comm;
	FILE *file;
	uint32_t field = 3U;

	snprintf(path, sizeof(path), "/proc/%ld/stat", (long)pid);
	file = fopen(path, "r");
	if(!file)
		return -1;
	if(!fgets(line, sizeof(line), file))
	{
		fclose(file);
		return -1;
	}
	fclose(file);
	end_comm = strrchr(line, ')');
	if(!end_comm || end_comm[1] != ' ')
		return -1;
	cursor = end_comm + 2;
	memset(ticks, 0, sizeof(*ticks));
	while(*cursor != '\0' && field <= 15U)
	{
		char *end;
		unsigned long long value;
		while(*cursor == ' ') cursor++;
		if(field == 3U)
		{
			if(*cursor == '\0') return -1;
			cursor++;
			field++;
			continue;
		}
		errno = 0;
		value = strtoull(cursor, &end, 10);
		if(errno != 0 || end == cursor)
			return -1;
		if(field == 14U) ticks->user = (uint64_t)value;
		if(field == 15U) ticks->system = (uint64_t)value;
		cursor = end;
		field++;
	}
	return field > 15U ? 0 : -1;
}

static uint64_t fnv_update(uint64_t hash, const void *data, size_t bytes)
{
	const uint8_t *input = (const uint8_t *)data;
	for(size_t i = 0U; i < bytes; i++)
	{
		hash ^= input[i];
		hash *= PG_BENCH_FNV_PRIME;
	}
	return hash;
}

static int execute_streaming(PGconn *connection,
							 const pg_bench_config_t *config,
							 uint64_t *rows,
							 uint64_t *fields,
							 uint64_t *bytes,
							 uint64_t *checksum,
							 uint64_t *csd_checksum,
							 uint32_t *csd_checksum_compatible)
{
	PGresult *result;
	if(PQsendQueryParams(connection, config->sql, 0, NULL, NULL, NULL, NULL,
							 config->binary ? 1 : 0) != 1 ||
	   PQsetSingleRowMode(connection) != 1)
	{
		fprintf(stderr, "query send failed: %s", PQerrorMessage(connection));
		return -1;
	}
	*rows = 0U;
	*fields = 0U;
	*bytes = 0U;
	*checksum = PG_BENCH_FNV_OFFSET;
	*csd_checksum = csd_canonical_checksum_begin();
	*csd_checksum_compatible = config->binary;
	while((result = PQgetResult(connection)) != NULL)
	{
		ExecStatusType status = PQresultStatus(result);
		if(status == PGRES_SINGLE_TUPLE)
		{
			int columns = PQnfields(result);
			uint32_t row_marker = UINT32_C(0x70677201);
			(*rows)++;
			*fields += (uint64_t)columns;
			*checksum = fnv_update(*checksum, &row_marker, sizeof(row_marker));
			csd_canonical_checksum_row(csd_checksum);
			for(int column = 0; column < columns; column++)
			{
				uint32_t is_null = (uint32_t)PQgetisnull(result, 0, column);
				uint32_t length = is_null ? 0U : (uint32_t)PQgetlength(result, 0, column);
				*checksum = fnv_update(*checksum, &is_null, sizeof(is_null));
				*checksum = fnv_update(*checksum, &length, sizeof(length));
				if(!is_null && length)
					*checksum = fnv_update(*checksum, PQgetvalue(result, 0, column), length);
				if(csd_canonical_checksum_pg_binary_field(csd_checksum, is_null,
					PQftype(result, column), is_null ? NULL : PQgetvalue(result, 0, column),
					length) < 0)
					*csd_checksum_compatible = 0U;
				*bytes += length;
			}
		}
		else if(status != PGRES_TUPLES_OK)
		{
			fprintf(stderr, "query failed: %s\n", PQresultErrorMessage(result));
			PQclear(result);
			while((result = PQgetResult(connection)) != NULL) PQclear(result);
			return -1;
		}
		PQclear(result);
	}
	return 0;
}

static int run_once(PGconn *connection,
					const pg_bench_config_t *config,
					uint32_t repetition,
					uint32_t report)
{
	pid_t backend_pid = (pid_t)PQbackendPID(connection);
	process_cpu_ticks_t backend_start;
	process_cpu_ticks_t backend_end;
	client_cpu_ms_t client_start;
	client_cpu_ms_t client_end;
	uint64_t rows;
	uint64_t fields;
	uint64_t bytes;
	uint64_t checksum;
	uint64_t csd_checksum;
	uint32_t csd_checksum_compatible;
	long ticks_per_second = sysconf(_SC_CLK_TCK);
	double start_ms;
	double elapsed_ms;

	if(backend_pid <= 0 || ticks_per_second <= 0 ||
	   read_process_cpu(backend_pid, &backend_start) < 0)
	{
		fprintf(stderr, "cannot read PostgreSQL backend CPU counters for pid=%ld\n", (long)backend_pid);
		return -1;
	}
	client_start = client_cpu_now();
	start_ms = monotonic_ms();
	if(execute_streaming(connection, config, &rows, &fields, &bytes, &checksum,
		&csd_checksum, &csd_checksum_compatible) < 0)
		return -1;
	elapsed_ms = monotonic_ms() - start_ms;
	client_end = client_cpu_now();
	if(read_process_cpu(backend_pid, &backend_end) < 0)
	{
		fprintf(stderr, "cannot read final PostgreSQL backend CPU counters\n");
		return -1;
	}
	if(report)
	{
		printf("{\"status\":\"ok\",\"engine\":\"postgresql\","
			"\"label\":\"%s\",\"repetition\":%u,\"binary\":%u,"
			"\"elapsed_ms\":%.3f,\"backend_user_ms\":%.3f,\"backend_system_ms\":%.3f,"
			"\"client_user_ms\":%.3f,\"client_system_ms\":%.3f,"
			"\"rows\":%" PRIu64 ",\"fields\":%" PRIu64 ",\"payload_bytes\":%" PRIu64 ","
			"\"checksum_fnv64\":\"%016" PRIx64 "\","
			"\"csd_canonical_checksum_fnv64\":\"%016" PRIx64 "\","
			"\"csd_canonical_checksum_compatible\":%u}\n",
			config->label, repetition, config->binary, elapsed_ms,
			(double)(backend_end.user - backend_start.user) * 1000.0 / ticks_per_second,
			(double)(backend_end.system - backend_start.system) * 1000.0 / ticks_per_second,
			client_end.user - client_start.user, client_end.system - client_start.system,
			rows, fields, bytes, checksum, csd_checksum, csd_checksum_compatible);
		fflush(stdout);
	}
	return 0;
}

int main(int argc, char **argv)
{
	pg_bench_config_t config;
	PGconn *connection;
	if(parse_args(argc, argv, &config) < 0)
	{
		usage();
		return 2;
	}
	connection = PQconnectdb(config.conninfo);
	if(!connection || PQstatus(connection) != CONNECTION_OK)
	{
		fprintf(stderr, "PostgreSQL connection failed: %s",
			connection ? PQerrorMessage(connection) : "out of memory\n");
		if(connection) PQfinish(connection);
		return 1;
	}
	for(uint32_t i = 0U; i < config.warmup; i++)
		if(run_once(connection, &config, i, 0U) < 0)
		{
			PQfinish(connection);
			return 1;
		}
	for(uint32_t i = 0U; i < config.repetitions; i++)
		if(run_once(connection, &config, i, 1U) < 0)
		{
			PQfinish(connection);
			return 1;
		}
	PQfinish(connection);
	return 0;
}
