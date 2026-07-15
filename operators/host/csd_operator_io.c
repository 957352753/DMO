#define _GNU_SOURCE

#include "csd_operator_io.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__linux__) && !defined(CSD_OPERATOR_IO_MOCK)
#include <linux/fiemap.h>
#include <linux/fs.h>
#include <linux/nvme_ioctl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#else
#ifdef _WIN32
#include <io.h>
#define close _close
#else
#include <unistd.h>
#endif
#endif

#define CSD_OPERATOR_FILTER_SEND_OPCODE 0xcdU
#define CSD_OPERATOR_FILTER_RESULT_OPCODE 0xceU
#define CSD_OPERATOR_NSID 1U
#define CSD_OPERATOR_COMMAND_PAGE_COUNT 32U
#define CSD_OPERATOR_LBAS_PER_PAGE (CSD_OPERATOR_PAGE_BYTES / CSD_OPERATOR_LBA_BYTES)
#define CSD_OPERATOR_FIEMAP_BATCH 256U

typedef char csd_tuple_desc_size_must_be_64[
	(sizeof(csd_operator_tuple_desc_t) == 64U) ? 1 : -1];
typedef char csd_qual_size_must_be_268[
	(sizeof(csd_operator_qual_t) == 268U) ? 1 : -1];
typedef char csd_extent_size_must_be_16[
	(sizeof(csd_operator_extent_t) == 16U) ? 1 : -1];

static void set_error(char *error, size_t error_bytes, const char *format, ...)
{
	va_list args;

	if(!error || error_bytes == 0U)
		return;
	va_start(args, format);
	(void)vsnprintf(error, error_bytes, format, args);
	va_end(args);
}

static int parse_i16(const char *text, int16_t *value, const char **end_out)
{
	char *end = NULL;
	long parsed;

	errno = 0;
	parsed = strtol(text, &end, 10);
	if(errno != 0 || end == text || parsed < INT16_MIN || parsed > INT16_MAX)
		return -1;
	*value = (int16_t)parsed;
	*end_out = end;
	return 0;
}

int csd_operator_parse_tuple_desc(const char *spec,
								  csd_operator_tuple_desc_t *desc,
								  uint32_t *natts)
{
	const char *cursor;
	uint32_t count = 0U;

	if(!spec || !desc || !natts || *spec == '\0')
		return -1;
	memset(desc, 0, sizeof(*desc));
	cursor = spec;
	while(*cursor != '\0')
	{
		const char *end;
		int16_t length;
		int16_t align_mask;

		if(count >= CSD_OPERATOR_MAX_COLUMNS ||
		   parse_i16(cursor, &length, &end) < 0 || *end != ':')
			return -1;
		cursor = end + 1;
		if(parse_i16(cursor, &align_mask, &end) < 0 || align_mask < 0 ||
		   (align_mask != 0 && align_mask != 1 && align_mask != 3 &&
			align_mask != 7))
			return -1;
		if(length == 0 || length < -1)
			return -1;
		desc->length[count] = length;
		desc->align_mask[count] = align_mask;
		count++;
		if(*end == '\0')
		{
			cursor = end;
			break;
		}
		if(*end != ',')
			return -1;
		cursor = end + 1;
		if(*cursor == '\0')
			return -1;
	}
	*natts = count;
	return count == 0U ? -1 : 0;
}

#if defined(__linux__) && !defined(CSD_OPERATOR_IO_MOCK)
static int extent_list_push(csd_operator_extent_list_t *list,
							uint64_t start_lba,
							uint64_t lba_count)
{
	if(list->count == list->capacity)
	{
		size_t capacity = list->capacity ? list->capacity * 2U : 64U;
		csd_operator_extent_t *items;

		if(capacity < list->capacity || capacity > SIZE_MAX / sizeof(*items))
			return -1;
		items = (csd_operator_extent_t *)realloc(
			list->items, capacity * sizeof(*items));
		if(!items)
			return -1;
		list->items = items;
		list->capacity = capacity;
	}
	list->items[list->count].start_lba = start_lba;
	list->items[list->count].lba_count = lba_count;
	list->count++;
	list->total_lbas += lba_count;
	return 0;
}
#endif

void csd_operator_extent_list_free(csd_operator_extent_list_t *list)
{
	if(!list)
		return;
	free(list->items);
	memset(list, 0, sizeof(*list));
}

#if defined(__linux__) && !defined(CSD_OPERATOR_IO_MOCK)
typedef struct raw_extent
{
	uint64_t logical;
	uint64_t physical;
	uint64_t length;
} raw_extent_t;

typedef struct raw_extent_list
{
	raw_extent_t *items;
	size_t count;
	size_t capacity;
} raw_extent_list_t;

static int validate_device_lba_size(const char *device_path,
								char *error,
								size_t error_bytes)
{
	unsigned int logical_bytes = 0U;
	int fd = open(device_path, O_RDONLY | O_CLOEXEC);
	if(fd < 0)
	{
		set_error(error, error_bytes, "cannot open target device %s: %s",
			device_path, strerror(errno));
		return -1;
	}
	if(ioctl(fd, BLKSSZGET, &logical_bytes) < 0)
	{
		set_error(error, error_bytes, "BLKSSZGET failed for %s: %s",
			device_path, strerror(errno));
		close(fd);
		return -1;
	}
	close(fd);
	if(logical_bytes != CSD_OPERATOR_LBA_BYTES)
	{
		set_error(error, error_bytes,
			"target logical block size is %u, firmware ABI requires %u",
			logical_bytes, CSD_OPERATOR_LBA_BYTES);
		return -1;
	}
	return 0;
}

static int raw_extent_push(raw_extent_list_t *list,
					   uint64_t logical,
					   uint64_t physical,
					   uint64_t length)
{
	if(list->count != 0U)
	{
		raw_extent_t *last = &list->items[list->count - 1U];
		if(last->logical + last->length == logical &&
		   last->physical + last->length == physical)
		{
			if(UINT64_MAX - last->length < length)
				return -1;
			last->length += length;
			return 0;
		}
	}
	if(list->count == list->capacity)
	{
		size_t capacity = list->capacity ? list->capacity * 2U : 32U;
		raw_extent_t *items;

		if(capacity < list->capacity || capacity > SIZE_MAX / sizeof(*items))
			return -1;
		items = (raw_extent_t *)realloc(list->items, capacity * sizeof(*items));
		if(!items)
			return -1;
		list->items = items;
		list->capacity = capacity;
	}
	list->items[list->count].logical = logical;
	list->items[list->count].physical = physical;
	list->items[list->count].length = length;
	list->count++;
	return 0;
}

static int collect_file_raw_extents(int fd,
							 const char *path,
							 uint64_t file_bytes,
							 raw_extent_list_t *raw,
							 char *error,
							 size_t error_bytes)
{
	uint64_t cursor = 0U;
	uint64_t covered = 0U;
	const uint32_t rejected_flags = FIEMAP_EXTENT_UNKNOWN |
		FIEMAP_EXTENT_DELALLOC | FIEMAP_EXTENT_ENCODED |
		FIEMAP_EXTENT_DATA_ENCRYPTED | FIEMAP_EXTENT_NOT_ALIGNED |
		FIEMAP_EXTENT_DATA_INLINE | FIEMAP_EXTENT_UNWRITTEN;

	while(cursor < file_bytes)
	{
		size_t bytes = sizeof(struct fiemap) +
			CSD_OPERATOR_FIEMAP_BATCH * sizeof(struct fiemap_extent);
		struct fiemap *map = (struct fiemap *)calloc(1U, bytes);
		uint32_t i;
		uint32_t last_flags = 0U;

		if(!map)
		{
			set_error(error, error_bytes, "out of memory collecting FIEMAP for %s", path);
			return -1;
		}
		map->fm_start = cursor;
		map->fm_length = file_bytes - cursor;
		map->fm_extent_count = CSD_OPERATOR_FIEMAP_BATCH;
		if(ioctl(fd, FS_IOC_FIEMAP, map) < 0)
		{
			set_error(error, error_bytes, "FIEMAP failed for %s: %s", path,
				strerror(errno));
			free(map);
			return -1;
		}
		if(map->fm_mapped_extents == 0U)
		{
			set_error(error, error_bytes, "FIEMAP left an unmapped tail in %s", path);
			free(map);
			return -1;
		}
		for(i = 0U; i < map->fm_mapped_extents; i++)
		{
			const struct fiemap_extent *extent = &map->fm_extents[i];
			uint64_t end;

			if((extent->fe_flags & rejected_flags) != 0U || extent->fe_length == 0U ||
			   extent->fe_logical != covered ||
			   UINT64_MAX - extent->fe_logical < extent->fe_length)
			{
				set_error(error, error_bytes,
					"unsafe or non-contiguous FIEMAP extent in %s at logical=%llu flags=0x%x",
					path, (unsigned long long)extent->fe_logical, extent->fe_flags);
				free(map);
				return -1;
			}
			end = extent->fe_logical + extent->fe_length;
			if(end > file_bytes ||
			   raw_extent_push(raw, extent->fe_logical, extent->fe_physical,
							   extent->fe_length) < 0)
			{
				set_error(error, error_bytes, "invalid or excessive FIEMAP data for %s", path);
				free(map);
				return -1;
			}
			covered = end;
			cursor = end;
			last_flags = extent->fe_flags;
		}
		free(map);
		if((last_flags & FIEMAP_EXTENT_LAST) != 0U)
			break;
	}
	if(covered != file_bytes)
	{
		set_error(error, error_bytes, "FIEMAP coverage mismatch for %s: file=%llu mapped=%llu",
			path, (unsigned long long)file_bytes, (unsigned long long)covered);
		return -1;
	}
	return 0;
}

int csd_operator_collect_extents(const char *device_path,
							 const char *const *file_paths,
							 size_t file_count,
							 uint32_t max_lbas_per_request,
							 csd_operator_extent_list_t *out,
							 char *error,
							 size_t error_bytes)
{
	struct stat device_stat;
	size_t file_index;
	uint32_t split_lbas;

	if(!device_path || !file_paths || file_count == 0U || !out ||
	   file_count > CSD_OPERATOR_MAX_FILES)
	{
		set_error(error, error_bytes, "invalid extent collection arguments");
		return -1;
	}
	memset(out, 0, sizeof(*out));
	if(stat(device_path, &device_stat) < 0 || !S_ISBLK(device_stat.st_mode))
	{
		set_error(error, error_bytes, "target is not a block device: %s", device_path);
		return -1;
	}
	if(validate_device_lba_size(device_path, error, error_bytes) < 0)
		return -1;
	split_lbas = max_lbas_per_request -
		(max_lbas_per_request % CSD_OPERATOR_LBAS_PER_PAGE);
	if(split_lbas < CSD_OPERATOR_LBAS_PER_PAGE)
	{
		set_error(error, error_bytes, "split LBA count must cover at least one 8 KiB page");
		return -1;
	}

	for(file_index = 0U; file_index < file_count; file_index++)
	{
		const char *path = file_paths[file_index];
		struct stat file_stat;
		raw_extent_list_t raw = {0};
		int fd = -1;
		size_t i;

		fd = open(path, O_RDONLY | O_CLOEXEC);
		if(fd < 0 || fstat(fd, &file_stat) < 0 || !S_ISREG(file_stat.st_mode))
		{
			set_error(error, error_bytes, "cannot open regular relation file %s: %s",
				path, strerror(errno));
			if(fd >= 0)
				close(fd);
			csd_operator_extent_list_free(out);
			return -1;
		}
		if(major(file_stat.st_dev) != major(device_stat.st_rdev) ||
		   minor(file_stat.st_dev) != minor(device_stat.st_rdev))
		{
			set_error(error, error_bytes,
				"relation file is not on target device: %s file_dev=%u:%u target=%u:%u",
				path, major(file_stat.st_dev), minor(file_stat.st_dev),
				major(device_stat.st_rdev), minor(device_stat.st_rdev));
			close(fd);
			csd_operator_extent_list_free(out);
			return -1;
		}
		if(file_stat.st_size <= 0 ||
		   ((uint64_t)file_stat.st_size % CSD_OPERATOR_PAGE_BYTES) != 0U ||
		   collect_file_raw_extents(fd, path, (uint64_t)file_stat.st_size,
								&raw, error, error_bytes) < 0)
		{
			if(error && error[0] == '\0')
				set_error(error, error_bytes, "relation file size is not 8 KiB aligned: %s", path);
			free(raw.items);
			close(fd);
			csd_operator_extent_list_free(out);
			return -1;
		}
		close(fd);

		for(i = 0U; i < raw.count; i++)
		{
			uint64_t start_lba;
			uint64_t remaining;

			if((raw.items[i].logical % CSD_OPERATOR_PAGE_BYTES) != 0U ||
			   (raw.items[i].physical % CSD_OPERATOR_LBA_BYTES) != 0U ||
			   (raw.items[i].length % CSD_OPERATOR_PAGE_BYTES) != 0U)
			{
				set_error(error, error_bytes,
					"FIEMAP extent splits a PostgreSQL page in %s at logical=%llu physical=%llu length=%llu",
					path, (unsigned long long)raw.items[i].logical,
					(unsigned long long)raw.items[i].physical,
					(unsigned long long)raw.items[i].length);
				free(raw.items);
				csd_operator_extent_list_free(out);
				return -1;
			}
			start_lba = raw.items[i].physical / CSD_OPERATOR_LBA_BYTES;
			remaining = raw.items[i].length / CSD_OPERATOR_LBA_BYTES;
			while(remaining != 0U)
			{
				uint64_t count = remaining < split_lbas ? remaining : split_lbas;
				if(extent_list_push(out, start_lba, count) < 0)
				{
					set_error(error, error_bytes, "out of memory splitting relation extents");
					free(raw.items);
					csd_operator_extent_list_free(out);
					return -1;
				}
				start_lba += count;
				remaining -= count;
			}
		}
		free(raw.items);
	}
	return 0;
}
#else
int csd_operator_collect_extents(const char *device_path,
							 const char *const *file_paths,
							 size_t file_count,
							 uint32_t max_lbas_per_request,
							 csd_operator_extent_list_t *out,
							 char *error,
							 size_t error_bytes)
{
	(void)device_path;
	(void)file_paths;
	(void)file_count;
	(void)max_lbas_per_request;
	if(out)
		memset(out, 0, sizeof(*out));
	set_error(error, error_bytes, "FIEMAP collection requires Linux");
	return -1;
}
#endif

void csd_operator_prepare_metadata(uint8_t metadata[CSD_OPERATOR_METADATA_BYTES],
							   const csd_operator_tuple_desc_t *desc,
							   uint32_t natts,
							   const csd_operator_extent_t *extent)
{
	csd_operator_qual_t *qual;
	csd_operator_extent_t *wire_extent;

	memset(metadata, 0, CSD_OPERATOR_METADATA_BYTES);
	memcpy(metadata, desc, sizeof(*desc));
	qual = (csd_operator_qual_t *)(metadata + 2U * 4096U);
	qual->natts = (int32_t)natts;
	qual->filter_count = 0;
	qual->file_count = 0;
	wire_extent = (csd_operator_extent_t *)(metadata + 3U * 4096U);
	*wire_extent = *extent;
}

int csd_operator_open_device(const char *device_path, char *error, size_t error_bytes)
{
#if defined(__linux__) && !defined(CSD_OPERATOR_IO_MOCK)
	struct stat info;
	int fd;

	if(!device_path || stat(device_path, &info) < 0 || !S_ISBLK(info.st_mode))
	{
		set_error(error, error_bytes, "not a block device: %s", device_path ? device_path : "(null)");
		return -1;
	}
	if(validate_device_lba_size(device_path, error, error_bytes) < 0)
		return -1;
	fd = open(device_path, O_RDWR | O_CLOEXEC);
	if(fd < 0)
		set_error(error, error_bytes, "cannot open %s: %s", device_path, strerror(errno));
	return fd;
#else
	(void)device_path;
	set_error(error, error_bytes, "NVMe passthrough requires Linux");
	return -1;
#endif
}

#if defined(__linux__) && !defined(CSD_OPERATOR_IO_MOCK)
static int nvme_passthrough(int fd,
						uint8_t opcode,
						void *buffer,
						uint32_t bytes,
						uint32_t timeout_ms,
						char *error,
						size_t error_bytes)
{
	struct nvme_passthru_cmd command;
	int rc;

	memset(&command, 0, sizeof(command));
	command.opcode = opcode;
	command.nsid = CSD_OPERATOR_NSID;
	command.addr = (uint64_t)(uintptr_t)buffer;
	command.data_len = bytes;
	command.cdw12 = CSD_OPERATOR_COMMAND_PAGE_COUNT;
	command.timeout_ms = timeout_ms;
	rc = ioctl(fd, NVME_IOCTL_IO_CMD, &command);
	if(rc != 0)
	{
		if(rc < 0)
			set_error(error, error_bytes,
				"NVMe opcode 0x%02x ioctl failed: errno=%d %s result=0x%x",
				opcode, errno, strerror(errno), command.result);
		else
			set_error(error, error_bytes,
				"NVMe opcode 0x%02x returned status=0x%x result=0x%x",
				opcode, (unsigned int)rc, command.result);
		return -1;
	}
	return 0;
}
#endif

int csd_operator_execute(int device_fd,
					 const uint8_t metadata[CSD_OPERATOR_METADATA_BYTES],
					 uint8_t *result,
					 size_t result_bytes,
					 uint32_t timeout_ms,
					 char *error,
					 size_t error_bytes)
{
#if defined(__linux__) && !defined(CSD_OPERATOR_IO_MOCK)
	uint32_t pages;

	if(device_fd < 0 || !metadata || !result ||
	   result_bytes < CSD_OPERATOR_PAGE_BYTES || result_bytes > UINT32_MAX ||
	   (result_bytes % CSD_OPERATOR_PAGE_BYTES) != 0U)
	{
		set_error(error, error_bytes, "invalid CSD operator execution arguments");
		return -1;
	}
	memset(result, 0, result_bytes);
	if(nvme_passthrough(device_fd, CSD_OPERATOR_FILTER_SEND_OPCODE,
						 (void *)metadata, CSD_OPERATOR_METADATA_BYTES,
						 timeout_ms, error, error_bytes) < 0 ||
	   nvme_passthrough(device_fd, CSD_OPERATOR_FILTER_RESULT_OPCODE,
						 result, (uint32_t)result_bytes,
						 timeout_ms, error, error_bytes) < 0)
		return -1;
	memcpy(&pages, result, sizeof(pages));
	if(pages == 0U || pages > result_bytes / CSD_OPERATOR_PAGE_BYTES)
	{
		set_error(error, error_bytes, "invalid CSD result page count: %u", pages);
		return -1;
	}
	return 0;
#else
	(void)device_fd;
	(void)metadata;
	(void)result;
	(void)result_bytes;
	(void)timeout_ms;
	set_error(error, error_bytes, "NVMe passthrough requires Linux");
	return -1;
#endif
}

double csd_operator_monotonic_ms(void)
{
#if defined(_WIN32)
	return (double)clock() * 1000.0 / (double)CLOCKS_PER_SEC;
#else
	struct timespec now;
	if(clock_gettime(CLOCK_MONOTONIC, &now) != 0)
		return 0.0;
	return (double)now.tv_sec * 1000.0 + (double)now.tv_nsec / 1000000.0;
#endif
}
