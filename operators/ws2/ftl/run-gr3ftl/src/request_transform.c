//////////////////////////////////////////////////////////////////////////////////
// request_transform.c for Cosmos+ OpenSSD
// Copyright (c) 2017 Hanyang University ENC Lab.
// Contributed by Yong Ho Song <yhsong@enc.hanyang.ac.kr>
//				  Jaewook Kwak <jwkwak@enc.hanyang.ac.kr>
//			      Sangjin Lee <sjlee@enc.hanyang.ac.kr>
//
// This file is part of Cosmos+ OpenSSD.
//
// Cosmos+ OpenSSD is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// Cosmos+ OpenSSD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Cosmos+ OpenSSD; see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Company: ENC Lab. <http://enc.hanyang.ac.kr>
// Engineer: Jaewook Kwak <jwkwak@enc.hanyang.ac.kr>
//
// Project Name: Cosmos+ OpenSSD
// Design Name: Cosmos+ Firmware
// Module Name: Request Scheduler
// File Name: request_transform.c
//
// Version: v1.0.0
//
// Description:
//	 - transform request information
//   - check dependency between requests
//   - issue host DMA request to host DMA engine
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Revision History:
//
// * v1.0.0
//   - First draft
//////////////////////////////////////////////////////////////////////////////////


#include "xil_printf.h"
#include <assert.h>
#include "nvme/nvme.h"
#include "nvme/host_lld.h"
#include "memory_map.h"
#include "filter_config.h"
#include "ftl_config.h"
#include "request_schedule.h"
#include "nvme/filter_common.h"
#include "nvme/filter_accel.h"
#include "nvme/filter_compute.h"
#include "nvme/filter_join.h"
#include "nvme/filter_projection.h"
#include "xil_types.h"
#include "xiltimer.h"
#include <stdint.h>
#include <string.h>
P_ROW_ADDR_DEPENDENCY_TABLE rowAddrDependencyTablePtr;

#define FILTER_STATS_MAGIC 0x46505354U
#define FILTER_PROFILE_MAGIC 0x46505246U
#define FILTER_PROFILE_VERSION 7U
#define FILTER_STATS_MAGIC_OFFSET 4U
#define FILTER_STATS_TOTAL_OFFSET 8U
#define FILTER_STATS_FPGA_OFFSET 9U
#define FILTER_STATS_FALLBACK_OFFSET 10U
#define FILTER_STATS_INVALID_OFFSET 11U
#define FILTER_COUNT_RESULT_MAGIC 0x46434e54U
#define FILTER_COUNT_RESULT_MAGIC_OFFSET 64U
#define FILTER_COUNT_RESULT_VERSION_OFFSET 68U
#define FILTER_COUNT_RESULT_VALUE_OFFSET 72U

#ifndef FILTER_ACCEL_TRACE_ENABLE
#define FILTER_ACCEL_TRACE_ENABLE 0
#endif

#ifndef FILTER_STATS_TRACE_ENABLE
#define FILTER_STATS_TRACE_ENABLE 0
#endif

static volatile unsigned int filter_force_arm_fallback_runtime =
	FILTER_FORCE_ARM_FALLBACK ? 1U : 0U;

static inline int filter_force_arm_fallback_enabled(void)
{
	return filter_force_arm_fallback_runtime != 0U;
}

#ifndef FILTER_READY_QUEUE_CAPACITY
#define FILTER_READY_QUEUE_CAPACITY AVAILABLE_DATA_BUFFER_ENTRY_COUNT
#endif

#if FILTER_ACCEL_TRACE_ENABLE
#define FILTER_TRACE(...) xil_printf(__VA_ARGS__)
#else
#define FILTER_TRACE(...) do { } while(0)
#endif

static unsigned short filter_tup_off_array[MAX_TUPLE_PER_PAGE];
static unsigned short filter_tup_len_array[MAX_TUPLE_PER_PAGE];
static unsigned short filter_pos_array[MAX_TUPLE_PER_PAGE * MAX_COL];
static unsigned short filter_len_array[MAX_TUPLE_PER_PAGE * MAX_COL];
static int filter_hit_off[MAX_TUPLE_PER_PAGE];
static unsigned short filter_hit_tuple_off[MAX_TUPLE_PER_PAGE];
static unsigned short filter_hit_tuple_len[MAX_TUPLE_PER_PAGE];
static unsigned short filter_fpga_tuple_off[MAX_TUPLE_PER_PAGE];
static unsigned short filter_fpga_tuple_len[MAX_TUPLE_PER_PAGE];
#if FILTER_ACCEL_VERIFY_ENABLE
static unsigned short filter_arm_tuple_off[MAX_TUPLE_PER_PAGE];
static unsigned short filter_arm_tuple_len[MAX_TUPLE_PER_PAGE];
#endif
static unsigned short filter_page_tuple_off[MAX_TUPLE_PER_PAGE];
static unsigned short filter_page_tuple_len[MAX_TUPLE_PER_PAGE];
static unsigned int filter_count_only_mode;
static uint64_t filter_count_only_hits;

#if FILTER_DEFER_READY_ENABLE
static unsigned int filter_ready_req_queue[FILTER_READY_QUEUE_CAPACITY];
static unsigned int filter_ready_req_count;
static unsigned int filter_ready_next_start_index;
#endif

#if FILTER_ACCEL_ENABLE && FILTER_ACCEL_POLL_SERVICE_ENABLE
static unsigned int filter_accel_poll_service_active;
#endif

static void FilterAccelPollService(void);
void CheckDoneNvmeDmaReq();
#if FILTER_ACCEL_ENABLE && FILTER_DIRECT_COALESCE_ENABLE
static unsigned int FilterDrainDeferredReadyReqsCoalesced(unsigned int budget,
														  unsigned int force);
#endif

#if FILTER_ACCEL_ENABLE && FILTER_STAGING_BATCH_ENABLE
static unsigned int filter_batch_pending_pages;
static unsigned int filter_batch_result_full;
static unsigned short filter_batch_pd_lower[FILTER_STAGING_BATCH_PAGES];
static unsigned short filter_batch_tuple_len[FILTER_STAGING_BATCH_PAGES];
#endif

#if FILTER_ACCEL_ENABLE && FILTER_DIRECT_COALESCE_ENABLE
static unsigned int filter_coalesce_req_slots[FILTER_DIRECT_COALESCE_PAGES];
static unsigned int filter_coalesce_page_addr[FILTER_DIRECT_COALESCE_PAGES];
static unsigned short filter_coalesce_pd_lower[FILTER_DIRECT_COALESCE_PAGES];
static unsigned short filter_coalesce_tuple_len[FILTER_DIRECT_COALESCE_PAGES];
#if FILTER_DIRECT_ASYNC_ENABLE
static unsigned int filter_coalesce_async_active;
static unsigned int filter_coalesce_async_req_count;
static unsigned int filter_coalesce_async_page_count;
static unsigned int filter_coalesce_async_polls;
static uint64_t filter_coalesce_async_start;
#endif
#endif

typedef struct
{
	unsigned int total_pages;
	unsigned int fpga_pages;
	unsigned int fallback_pages;
	unsigned int invalid_pages;
	int last_accel_status;
	unsigned int last_hw_status;
	unsigned int last_hw_error;
	unsigned int batch_invocations;
	unsigned int batch_pages;
	uint64_t nand_read_cycles;
	uint64_t fpga_compute_cycles;
	uint64_t arm_fallback_cycles;
	uint64_t materialize_cycles;
	uint64_t dma_return_cycles;
	unsigned int verify_pages;
	unsigned int verify_mismatch_pages;
	unsigned int fpga_discard_pages;
	unsigned int verify_mismatch_tuples;
	uint64_t verify_cycles;
	uint64_t csd_total_cycles;
	uint64_t host_to_csd_dma_cycles;
	uint64_t csd_to_host_dma_cycles;
	uint64_t nand_read_wall_cycles;
	uint64_t nand_read_start_events;
	uint64_t nand_read_complete_events;
	unsigned int nand_read_pending_max;
	uint64_t nand_read_pending_sum;
	uint64_t nand_read_pending_samples;
	uint64_t filter_issue_calls;
	uint64_t filter_stage_calls;
	uint64_t filter_stage_pages;
	uint64_t filter_flush_calls;
	uint64_t filter_flush_pages;
	uint64_t filter_flush_while_nand_pending;
	uint64_t staging_copy_cycles;
	uint64_t filter_wait_loop_iterations;
	unsigned int filter_wait_loop_max;
	unsigned int filter_wait_timeouts;
	uint64_t filter_defer_enqueues;
	uint64_t filter_defer_drains;
	unsigned int filter_defer_max_depth;
	unsigned int filter_defer_overflows;
	uint64_t result_full_events;
	uint64_t result_dropped_tuples;
} filter_exec_stats_t;

static filter_exec_stats_t filter_exec_stats;
static filter_exec_stats_t filter_exec_stats_snapshot;
static unsigned int filter_exec_stats_snapshot_valid;

static uint64_t filter_nand_req_start[AVAILABLE_OUNTSTANDING_REQ_COUNT];
static uint64_t filter_csd_total_start;
static uint64_t filter_nand_read_wall_start;
static unsigned int filter_nand_read_pending;

void FilterProfileAddNandReadCycles(uint64_t cycles);

typedef struct
{
	uint32_t magic;
	uint32_t version;
	uint32_t struct_bytes;
	uint32_t reserved0;
	uint64_t timer_counts_per_second;
	uint64_t total_pages;
	uint64_t fpga_pages;
	uint64_t fallback_pages;
	uint64_t invalid_pages;
	int32_t last_accel_status;
	uint32_t last_hw_status;
	uint32_t last_hw_error;
	uint32_t batch_invocations;
	uint64_t batch_pages;
	uint64_t nand_read_cycles;
	uint64_t fpga_compute_cycles;
	uint64_t arm_fallback_cycles;
	uint64_t materialize_cycles;
	uint64_t dma_return_cycles;
	uint64_t verify_pages;
	uint64_t verify_mismatch_pages;
	uint64_t fpga_discard_pages;
	uint64_t verify_mismatch_tuples;
	uint64_t verify_cycles;
	uint64_t csd_total_cycles;
	uint64_t host_to_csd_dma_cycles;
	uint64_t csd_to_host_dma_cycles;
	uint64_t nand_read_wall_cycles;
	uint64_t nand_read_start_events;
	uint64_t nand_read_complete_events;
	uint32_t nand_read_pending_max;
	uint32_t reserved1;
	uint64_t nand_read_pending_sum;
	uint64_t nand_read_pending_samples;
	uint64_t filter_issue_calls;
	uint64_t filter_stage_calls;
	uint64_t filter_stage_pages;
	uint64_t filter_flush_calls;
	uint64_t filter_flush_pages;
	uint64_t filter_flush_while_nand_pending;
	uint64_t staging_copy_cycles;
	uint64_t filter_wait_loop_iterations;
	uint32_t filter_wait_loop_max;
	uint32_t filter_wait_timeouts;
	uint64_t filter_defer_enqueues;
	uint64_t filter_defer_drains;
	uint32_t filter_defer_max_depth;
	uint32_t filter_defer_overflows;
	uint64_t result_full_events;
	uint64_t result_dropped_tuples;
} filter_profile_result_t;

static unsigned char filter_stats_clamp_u8(unsigned int value)
{
	return (value > 255U) ? 255U : (unsigned char)value;
}

uint64_t FilterProfileNow(void)
{
	XTime now;
	XTime_GetTime(&now);
	return (uint64_t)now;
}

static void filter_profile_add_cycles(uint64_t *dst, uint64_t cycles)
{
	*dst += cycles;
}

static void filter_profile_sample_nand_pending(void)
{
	filter_exec_stats.nand_read_pending_samples++;
	filter_exec_stats.nand_read_pending_sum += filter_nand_read_pending;
	if(filter_nand_read_pending > filter_exec_stats.nand_read_pending_max)
		filter_exec_stats.nand_read_pending_max = filter_nand_read_pending;
}

static void FilterAccelPollService(void)
{
#if FILTER_ACCEL_ENABLE && FILTER_ACCEL_POLL_SERVICE_ENABLE
	if(filter_accel_poll_service_active)
		return;

	filter_accel_poll_service_active = 1;
	CheckDoneNvmeDmaReq();
	SchedulingNandReq();
	filter_accel_poll_service_active = 0;
#endif
}

void FilterExecStatsReset(void)
{
	memset(&filter_exec_stats, 0, sizeof(filter_exec_stats));
	memset(&filter_exec_stats_snapshot, 0, sizeof(filter_exec_stats_snapshot));
	memset(filter_nand_req_start, 0, sizeof(filter_nand_req_start));
	filter_csd_total_start = FilterProfileNow();
	filter_nand_read_wall_start = 0;
	filter_nand_read_pending = 0;
	filter_count_only_mode = 0;
	filter_count_only_hits = 0;
	filter_accel_set_count_only(0);
	filter_exec_stats.last_accel_status = FILTER_ACCEL_OK;
	filter_exec_stats_snapshot.last_accel_status = FILTER_ACCEL_OK;
	filter_exec_stats_snapshot_valid = 0;
#if FILTER_ACCEL_ENABLE && FILTER_STAGING_BATCH_ENABLE
	filter_batch_pending_pages = 0;
	filter_batch_result_full = 0;
#endif
#if FILTER_DEFER_READY_ENABLE
	filter_ready_req_count = 0;
	filter_ready_next_start_index = 0;
#endif
#if FILTER_ACCEL_ENABLE && FILTER_DIRECT_COALESCE_ENABLE && FILTER_DIRECT_ASYNC_ENABLE
	filter_coalesce_async_active = 0;
	filter_coalesce_async_req_count = 0;
	filter_coalesce_async_page_count = 0;
	filter_coalesce_async_polls = 0;
	filter_coalesce_async_start = 0;
#endif
#if FILTER_ACCEL_ENABLE && FILTER_ACCEL_POLL_SERVICE_ENABLE
	filter_accel_poll_service_active = 0;
	filter_accel_set_poll_hook(FilterAccelPollService, FILTER_ACCEL_POLL_SERVICE_INTERVAL);
#else
	filter_accel_set_poll_hook(0, 0);
#endif
}

void FilterSetCountOnlyMode(unsigned int enable)
{
	filter_count_only_mode = enable ? 1U : 0U;
	filter_count_only_hits = 0;
	filter_accel_set_count_only(filter_count_only_mode);
}

void FilterProfileStartNandRead(unsigned int reqSlotTag)
{
	uint64_t now;

	if(reqSlotTag >= AVAILABLE_OUNTSTANDING_REQ_COUNT ||
	   filter_nand_req_start[reqSlotTag] != 0)
		return;

	now = FilterProfileNow();
	filter_nand_req_start[reqSlotTag] = now;
	if(filter_nand_read_pending == 0)
		filter_nand_read_wall_start = now;
	filter_nand_read_pending++;
	filter_exec_stats.nand_read_start_events++;
	filter_profile_sample_nand_pending();
}

void FilterProfileCompleteNandRead(unsigned int reqSlotTag)
{
	if(reqSlotTag < AVAILABLE_OUNTSTANDING_REQ_COUNT && filter_nand_req_start[reqSlotTag] != 0)
	{
		uint64_t now = FilterProfileNow();
		FilterProfileAddNandReadCycles(now - filter_nand_req_start[reqSlotTag]);
		filter_nand_req_start[reqSlotTag] = 0;
		filter_exec_stats.nand_read_complete_events++;
		if(filter_nand_read_pending > 0)
		{
			filter_nand_read_pending--;
			filter_profile_sample_nand_pending();
			if(filter_nand_read_pending == 0 && filter_nand_read_wall_start != 0)
			{
				filter_profile_add_cycles(&filter_exec_stats.nand_read_wall_cycles,
										  now - filter_nand_read_wall_start);
				filter_nand_read_wall_start = 0;
			}
		}
	}
}

void FilterProfileAddNandReadCycles(uint64_t cycles)
{
	filter_profile_add_cycles(&filter_exec_stats.nand_read_cycles, cycles);
}

void FilterProfileAddFpgaComputeCycles(uint64_t cycles)
{
	filter_profile_add_cycles(&filter_exec_stats.fpga_compute_cycles, cycles);
}

void FilterProfileAddArmFallbackCycles(uint64_t cycles)
{
	filter_profile_add_cycles(&filter_exec_stats.arm_fallback_cycles, cycles);
}

void FilterProfileAddMaterializeCycles(uint64_t cycles)
{
	filter_profile_add_cycles(&filter_exec_stats.materialize_cycles, cycles);
}

void FilterProfileAddHostToCsdDmaCycles(uint64_t cycles)
{
	filter_profile_add_cycles(&filter_exec_stats.host_to_csd_dma_cycles, cycles);
}

void FilterProfileAddCsdToHostDmaCycles(uint64_t cycles)
{
	filter_profile_add_cycles(&filter_exec_stats.csd_to_host_dma_cycles, cycles);
}

void FilterProfileAddDmaReturnCycles(uint64_t cycles)
{
	filter_profile_add_cycles(&filter_exec_stats.dma_return_cycles, cycles);
	FilterProfileAddCsdToHostDmaCycles(cycles);
}

void FilterProfileAddStagingCopyCycles(uint64_t cycles)
{
	filter_profile_add_cycles(&filter_exec_stats.staging_copy_cycles, cycles);
}

void FilterProfileRecordFilterWaitLoop(unsigned int iterations, unsigned int timeout)
{
	filter_exec_stats.filter_wait_loop_iterations += iterations;
	if(iterations > filter_exec_stats.filter_wait_loop_max)
		filter_exec_stats.filter_wait_loop_max = iterations;
	if(timeout)
		filter_exec_stats.filter_wait_timeouts++;
}

#if FILTER_ACCEL_VERIFY_ENABLE
static void FilterProfileAddVerifyCycles(uint64_t cycles)
{
	filter_profile_add_cycles(&filter_exec_stats.verify_cycles, cycles);
}
#endif

void FilterProfileSnapshotCurrent(void)
{
	uint64_t now = FilterProfileNow();

	if(filter_csd_total_start != 0)
		filter_exec_stats.csd_total_cycles = now - filter_csd_total_start;
	filter_exec_stats_snapshot = filter_exec_stats;
	filter_exec_stats_snapshot_valid = 1;
}

void FilterProfileWriteBuffer(void *buffer, unsigned int bufferBytes)
{
	filter_profile_result_t profile;
	const filter_exec_stats_t *stats;

	if(buffer == 0 || bufferBytes < sizeof(profile))
		return;

	stats = filter_exec_stats_snapshot_valid ? &filter_exec_stats_snapshot : &filter_exec_stats;

	memset(buffer, 0, bufferBytes);
	memset(&profile, 0, sizeof(profile));
	profile.magic = FILTER_PROFILE_MAGIC;
	profile.version = FILTER_PROFILE_VERSION;
	profile.struct_bytes = sizeof(profile);
	profile.timer_counts_per_second = (uint64_t)COUNTS_PER_SECOND;
	profile.total_pages = stats->total_pages;
	profile.fpga_pages = stats->fpga_pages;
	profile.fallback_pages = stats->fallback_pages;
	profile.invalid_pages = stats->invalid_pages;
	profile.last_accel_status = stats->last_accel_status;
	profile.last_hw_status = stats->last_hw_status;
	profile.last_hw_error = stats->last_hw_error;
	profile.batch_invocations = stats->batch_invocations;
	profile.batch_pages = stats->batch_pages;
	profile.nand_read_cycles = stats->nand_read_cycles;
	profile.fpga_compute_cycles = stats->fpga_compute_cycles;
	profile.arm_fallback_cycles = stats->arm_fallback_cycles;
	profile.materialize_cycles = stats->materialize_cycles;
	profile.dma_return_cycles = stats->dma_return_cycles;
	profile.verify_pages = stats->verify_pages;
	profile.verify_mismatch_pages = stats->verify_mismatch_pages;
	profile.fpga_discard_pages = stats->fpga_discard_pages;
	profile.verify_mismatch_tuples = stats->verify_mismatch_tuples;
	profile.verify_cycles = stats->verify_cycles;
	profile.csd_total_cycles = stats->csd_total_cycles;
	profile.host_to_csd_dma_cycles = stats->host_to_csd_dma_cycles;
	profile.csd_to_host_dma_cycles = stats->csd_to_host_dma_cycles;
	profile.nand_read_wall_cycles = stats->nand_read_wall_cycles;
	profile.nand_read_start_events = stats->nand_read_start_events;
	profile.nand_read_complete_events = stats->nand_read_complete_events;
	profile.nand_read_pending_max = stats->nand_read_pending_max;
	profile.nand_read_pending_sum = stats->nand_read_pending_sum;
	profile.nand_read_pending_samples = stats->nand_read_pending_samples;
	profile.filter_issue_calls = stats->filter_issue_calls;
	profile.filter_stage_calls = stats->filter_stage_calls;
	profile.filter_stage_pages = stats->filter_stage_pages;
	profile.filter_flush_calls = stats->filter_flush_calls;
	profile.filter_flush_pages = stats->filter_flush_pages;
	profile.filter_flush_while_nand_pending = stats->filter_flush_while_nand_pending;
	profile.staging_copy_cycles = stats->staging_copy_cycles;
	profile.filter_wait_loop_iterations = stats->filter_wait_loop_iterations;
	profile.filter_wait_loop_max = stats->filter_wait_loop_max;
	profile.filter_wait_timeouts = stats->filter_wait_timeouts;
	profile.filter_defer_enqueues = stats->filter_defer_enqueues;
	profile.filter_defer_drains = stats->filter_defer_drains;
	profile.filter_defer_max_depth = stats->filter_defer_max_depth;
	profile.filter_defer_overflows = stats->filter_defer_overflows;
	profile.result_full_events = stats->result_full_events;
	profile.result_dropped_tuples = stats->result_dropped_tuples;
	memcpy(buffer, &profile, sizeof(profile));
}

unsigned int FilterProfileResultSize(void)
{
	return sizeof(filter_profile_result_t);
}

void FilterExecStatsWriteResult(void)
{
	unsigned char *result =
		(unsigned char *)(uintptr_t)FILTER_RESULT_DRAM_DATA_BUFFER;
	unsigned int magic = FILTER_STATS_MAGIC;
	unsigned int count_magic = FILTER_COUNT_RESULT_MAGIC;
	unsigned int count_version = 1U;

	memcpy(result + FILTER_STATS_MAGIC_OFFSET, &magic, sizeof(magic));
	result[FILTER_STATS_TOTAL_OFFSET] = filter_stats_clamp_u8(filter_exec_stats.total_pages);
	result[FILTER_STATS_FPGA_OFFSET] = filter_stats_clamp_u8(filter_exec_stats.fpga_pages);
	result[FILTER_STATS_FALLBACK_OFFSET] = filter_stats_clamp_u8(filter_exec_stats.fallback_pages);
	result[FILTER_STATS_INVALID_OFFSET] = filter_stats_clamp_u8(filter_exec_stats.invalid_pages);
	if(filter_count_only_mode)
	{
		memcpy(result + FILTER_COUNT_RESULT_MAGIC_OFFSET, &count_magic, sizeof(count_magic));
		memcpy(result + FILTER_COUNT_RESULT_VERSION_OFFSET, &count_version, sizeof(count_version));
		memcpy(result + FILTER_COUNT_RESULT_VALUE_OFFSET, &filter_count_only_hits, sizeof(filter_count_only_hits));
	}

#if FILTER_STATS_TRACE_ENABLE
	xil_printf("FILTER_STATS total=%u fpga=%u fallback=%u invalid=%u last_accel=%d hw_status=0x%08x error=0x%08x\r\n",
			   filter_exec_stats.total_pages,
			   filter_exec_stats.fpga_pages,
			   filter_exec_stats.fallback_pages,
			   filter_exec_stats.invalid_pages,
			   (int)filter_exec_stats.last_accel_status,
			   filter_exec_stats.last_hw_status,
			   filter_exec_stats.last_hw_error);
#endif
}

void FilterFinalizeResult(void)
{
	filter_compute_finalize();
	filter_join_finalize();
}

static void FilterIssueReadyReq(unsigned int reqSlotTag)
{
	IssueFilter(reqSlotTag);
	CompleteFilterReq(reqSlotTag);
}

#if FILTER_DEFER_READY_ENABLE
static unsigned int FilterReadyReqAt(unsigned int offset)
{
	assert(offset < filter_ready_req_count);
	return filter_ready_req_queue[offset];
}

static unsigned int FilterReadyReqStartIndex(unsigned int reqSlotTag)
{
	return reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.startIndex;
}

static unsigned int FilterReadyReqBlockCount(unsigned int reqSlotTag)
{
	return reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.numOfNvmeBlock;
}

static unsigned int FilterReadyReqIsNext(void)
{
	return filter_ready_req_count > 0 &&
		FilterReadyReqStartIndex(FilterReadyReqAt(0)) ==
			filter_ready_next_start_index;
}

static unsigned int FilterPopReadyReq(void)
{
	unsigned int reqSlotTag;
	unsigned int replacement;
	unsigned int pop_index = 0;

	assert(filter_ready_req_count > 0);
	reqSlotTag = filter_ready_req_queue[0];
	filter_ready_req_count--;
	if(filter_ready_req_count == 0)
		return reqSlotTag;

	replacement = filter_ready_req_queue[filter_ready_req_count];
	while(1)
	{
		unsigned int child = pop_index * 2U + 1U;

		if(child >= filter_ready_req_count)
			break;
		if(child + 1U < filter_ready_req_count &&
		   FilterReadyReqStartIndex(filter_ready_req_queue[child + 1U]) <
			FilterReadyReqStartIndex(filter_ready_req_queue[child]))
			child++;
		if(FilterReadyReqStartIndex(replacement) <=
		   FilterReadyReqStartIndex(filter_ready_req_queue[child]))
			break;
		filter_ready_req_queue[pop_index] = filter_ready_req_queue[child];
		pop_index = child;
	}
	filter_ready_req_queue[pop_index] = replacement;

	return reqSlotTag;
}

static void FilterInsertReadyReq(unsigned int reqSlotTag)
{
	unsigned int insert_index = filter_ready_req_count;
	unsigned int start_index = FilterReadyReqStartIndex(reqSlotTag);

	filter_ready_req_count++;
	while(insert_index > 0)
	{
		unsigned int parent = (insert_index - 1U) >> 1;
		unsigned int parent_req = filter_ready_req_queue[parent];

		if(FilterReadyReqStartIndex(parent_req) <= start_index)
			break;
		filter_ready_req_queue[insert_index] = parent_req;
		insert_index = parent;
	}
	filter_ready_req_queue[insert_index] = reqSlotTag;
}

static void FilterCompleteOrderedReadyReq(unsigned int reqSlotTag)
{
	unsigned int start_index = FilterReadyReqStartIndex(reqSlotTag);
	unsigned int block_count = FilterReadyReqBlockCount(reqSlotTag);
	unsigned int next_start_index;

	assert(start_index == filter_ready_next_start_index);
	assert(block_count > 0);
	next_start_index = start_index + block_count;
	CompleteFilterReq(reqSlotTag);
	filter_ready_next_start_index = next_start_index;
}

static void FilterIssueOrderedReadyReq(unsigned int reqSlotTag)
{
	unsigned int start_index = FilterReadyReqStartIndex(reqSlotTag);
	unsigned int block_count = FilterReadyReqBlockCount(reqSlotTag);
	unsigned int next_start_index;

	assert(start_index == filter_ready_next_start_index);
	assert(block_count > 0);
	next_start_index = start_index + block_count;
	IssueFilter(reqSlotTag);
	CompleteFilterReq(reqSlotTag);
	filter_ready_next_start_index = next_start_index;
}
#endif

static void FilterQueueReadyReq(unsigned int reqSlotTag)
{
#if FILTER_DEFER_READY_ENABLE
	if(!FILTER_TAG)
	{
		FilterIssueReadyReq(reqSlotTag);
		return;
	}

	if(filter_ready_req_count >= FILTER_READY_QUEUE_CAPACITY)
	{
		filter_exec_stats.filter_defer_overflows++;
		assert(!"filter ready request queue overflow");
		return;
	}

	reqPoolPtr->reqPool[reqSlotTag].reqQueueType = REQ_QUEUE_TYPE_NONE;
	reqPoolPtr->reqPool[reqSlotTag].prevReq = REQ_SLOT_TAG_NONE;
	reqPoolPtr->reqPool[reqSlotTag].nextReq = REQ_SLOT_TAG_NONE;
	FilterInsertReadyReq(reqSlotTag);
	filter_exec_stats.filter_defer_enqueues++;
	if(filter_ready_req_count > filter_exec_stats.filter_defer_max_depth)
		filter_exec_stats.filter_defer_max_depth = filter_ready_req_count;
#else
	FilterIssueReadyReq(reqSlotTag);
#endif
}

void FilterDrainDeferredReadyReqs(void)
{
#if FILTER_DEFER_READY_ENABLE
	while(FilterHasPendingWork())
	{
#if FILTER_ACCEL_ENABLE && FILTER_DIRECT_COALESCE_ENABLE
		if(FilterDrainDeferredReadyReqsCoalesced(FILTER_READY_QUEUE_CAPACITY, 1) == 0)
			break;
#else
		if(!FilterReadyReqIsNext())
			break;
		unsigned int reqSlotTag = FilterPopReadyReq();
		filter_exec_stats.filter_defer_drains++;

		FilterIssueOrderedReadyReq(reqSlotTag);
#endif
	}
#endif
}

unsigned int FilterHasPendingWork(void)
{
#if FILTER_DEFER_READY_ENABLE
	if(filter_ready_req_count > 0)
		return 1;
#endif
#if FILTER_ACCEL_ENABLE && FILTER_DIRECT_COALESCE_ENABLE && FILTER_DIRECT_ASYNC_ENABLE
	if(filter_coalesce_async_active)
		return 1;
#endif
#if FILTER_ACCEL_ENABLE && FILTER_STAGING_BATCH_ENABLE
	if(filter_batch_pending_pages > 0)
		return 1;
#endif
	return 0;
}

void FilterDrainDeferredReadyReqsAdaptive(unsigned int force)
{
#if FILTER_DEFER_READY_ENABLE
	unsigned int drained = 0;

	if(force)
	{
		FilterDrainDeferredReadyReqs();
		return;
	}

#if FILTER_ACCEL_ENABLE && FILTER_DIRECT_COALESCE_ENABLE && FILTER_DIRECT_ASYNC_ENABLE
	if(filter_coalesce_async_active)
	{
		(void)FilterDrainDeferredReadyReqsCoalesced(FILTER_DEFER_DRAIN_BUDGET, 0);
		return;
	}
#endif

	if(filter_ready_req_count == 0)
		return;
	if(!FilterReadyReqIsNext())
		return;

	if(filter_ready_req_count < FILTER_DEFER_DRAIN_HIGH_WATERMARK &&
	   filter_nand_read_pending > FILTER_DEFER_DRAIN_LOW_WATERMARK)
		return;

	while(filter_ready_req_count > 0 && drained < FILTER_DEFER_DRAIN_BUDGET)
	{
#if FILTER_ACCEL_ENABLE && FILTER_DIRECT_COALESCE_ENABLE
		unsigned int drain_budget = FILTER_DEFER_DRAIN_BUDGET - drained;
		unsigned int drained_now = FilterDrainDeferredReadyReqsCoalesced(drain_budget, 0);

		if(drained_now == 0)
			break;
		drained += drained_now;
#else
		unsigned int reqSlotTag = FilterPopReadyReq();
		filter_exec_stats.filter_defer_drains++;
		drained++;

		FilterIssueOrderedReadyReq(reqSlotTag);
#endif
	}
#else
	(void)force;
#endif
}

void InitDependencyTable()
{
	unsigned int blockNo, wayNo, chNo;
	rowAddrDependencyTablePtr = (P_ROW_ADDR_DEPENDENCY_TABLE)ROW_ADDR_DEPENDENCY_TABLE_ADDR;

	for(blockNo=0 ; blockNo<MAIN_BLOCKS_PER_DIE ; blockNo++)
	{
		for(wayNo=0 ; wayNo<USER_WAYS ; wayNo++)
		{
			for(chNo=0 ; chNo<USER_CHANNELS ; chNo++)
			{
				rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].permittedProgPage = 0;
				rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].blockedReadReqCnt = 0;
				rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].blockedEraseReqFlag = 0;
			}
		}
	}
}

void ReqTransNvmeToSlice(unsigned int cmdSlotTag, unsigned int startLba, unsigned int nlb, unsigned int cmdCode)
{
	unsigned int reqSlotTag, requestedNvmeBlock, tempNumOfNvmeBlock, transCounter, tempLsa, loop, nvmeBlockOffset, nvmeDmaStartIndex, reqCode;

	requestedNvmeBlock = nlb + 1;
	transCounter = 0;
	nvmeDmaStartIndex = 0;
	tempLsa = startLba / NVME_BLOCKS_PER_SLICE;
	loop = ((startLba % NVME_BLOCKS_PER_SLICE) + requestedNvmeBlock) / NVME_BLOCKS_PER_SLICE;

	if(cmdCode == IO_NVM_WRITE)
		reqCode = REQ_CODE_WRITE;
	else if(cmdCode == IO_NVM_READ)
		reqCode = REQ_CODE_READ;
	else
		assert(!"[WARNING] Not supported command code [WARNING]");

	//first transform
	nvmeBlockOffset = (startLba % NVME_BLOCKS_PER_SLICE);
	if(loop)
		tempNumOfNvmeBlock = NVME_BLOCKS_PER_SLICE - nvmeBlockOffset;
	else
		tempNumOfNvmeBlock = requestedNvmeBlock;

	reqSlotTag = GetFromFreeReqQ();

	reqPoolPtr->reqPool[reqSlotTag].reqType = REQ_TYPE_SLICE;
	reqPoolPtr->reqPool[reqSlotTag].reqCode = reqCode;
	reqPoolPtr->reqPool[reqSlotTag].nvmeCmdSlotTag = cmdSlotTag;
	reqPoolPtr->reqPool[reqSlotTag].logicalSliceAddr = tempLsa;
	reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.startIndex = nvmeDmaStartIndex;
	reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.nvmeBlockOffset = nvmeBlockOffset;
	reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.numOfNvmeBlock = tempNumOfNvmeBlock;

	PutToSliceReqQ(reqSlotTag);

	tempLsa++;
	transCounter++;
	nvmeDmaStartIndex += tempNumOfNvmeBlock;

	//transform continue
	while(transCounter < loop)
	{
		nvmeBlockOffset = 0;
		tempNumOfNvmeBlock = NVME_BLOCKS_PER_SLICE;

		reqSlotTag = GetFromFreeReqQ();

		reqPoolPtr->reqPool[reqSlotTag].reqType = REQ_TYPE_SLICE;
		reqPoolPtr->reqPool[reqSlotTag].reqCode = reqCode;
		reqPoolPtr->reqPool[reqSlotTag].nvmeCmdSlotTag = cmdSlotTag;
		reqPoolPtr->reqPool[reqSlotTag].logicalSliceAddr = tempLsa;
		reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.startIndex = nvmeDmaStartIndex;
		reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.nvmeBlockOffset = nvmeBlockOffset;
		reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.numOfNvmeBlock = tempNumOfNvmeBlock;

		PutToSliceReqQ(reqSlotTag);

		tempLsa++;
		transCounter++;
		nvmeDmaStartIndex += tempNumOfNvmeBlock;
	}

	//last transform
	nvmeBlockOffset = 0;
	tempNumOfNvmeBlock = (startLba + requestedNvmeBlock) % NVME_BLOCKS_PER_SLICE;
	if((tempNumOfNvmeBlock == 0) || (loop == 0))
		return ;

	reqSlotTag = GetFromFreeReqQ();

	reqPoolPtr->reqPool[reqSlotTag].reqType = REQ_TYPE_SLICE;
	reqPoolPtr->reqPool[reqSlotTag].reqCode = reqCode;
	reqPoolPtr->reqPool[reqSlotTag].nvmeCmdSlotTag = cmdSlotTag;
	reqPoolPtr->reqPool[reqSlotTag].logicalSliceAddr = tempLsa;
	reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.startIndex = nvmeDmaStartIndex;
	reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.nvmeBlockOffset = nvmeBlockOffset;
	reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.numOfNvmeBlock = tempNumOfNvmeBlock;

	PutToSliceReqQ(reqSlotTag);
}



static unsigned int ScheduleDataBufWriteback(unsigned int dataBufEntry,
											 unsigned int cmdSlotTag)
{
	unsigned int reqSlotTag, virtualSliceAddr;

	if(dataBufMapPtr->dataBuf[dataBufEntry].dirty != DATA_BUF_DIRTY ||
	   dataBufMapPtr->dataBuf[dataBufEntry].logicalSliceAddr == LSA_NONE)
		return 0;

	reqSlotTag = GetFromFreeReqQ();
	virtualSliceAddr = AddrTransWrite(dataBufMapPtr->dataBuf[dataBufEntry].logicalSliceAddr);

	reqPoolPtr->reqPool[reqSlotTag].reqType = REQ_TYPE_NAND;
	reqPoolPtr->reqPool[reqSlotTag].reqCode = REQ_CODE_WRITE;
	reqPoolPtr->reqPool[reqSlotTag].nvmeCmdSlotTag = cmdSlotTag;
	reqPoolPtr->reqPool[reqSlotTag].logicalSliceAddr =
		dataBufMapPtr->dataBuf[dataBufEntry].logicalSliceAddr;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.dataBufFormat = REQ_OPT_DATA_BUF_ENTRY;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandAddr = REQ_OPT_NAND_ADDR_VSA;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandEcc = REQ_OPT_NAND_ECC_ON;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandEccWarning = REQ_OPT_NAND_ECC_WARNING_ON;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.rowAddrDependencyCheck =
		REQ_OPT_ROW_ADDR_DEPENDENCY_CHECK;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.blockSpace = REQ_OPT_BLOCK_SPACE_MAIN;
	reqPoolPtr->reqPool[reqSlotTag].dataBufInfo.entry = dataBufEntry;
	UpdateDataBufEntryInfoBlockingReq(dataBufEntry, reqSlotTag);
	reqPoolPtr->reqPool[reqSlotTag].nandInfo.virtualSliceAddr = virtualSliceAddr;

	SelectLowLevelReqQ(reqSlotTag);
	dataBufMapPtr->dataBuf[dataBufEntry].dirty = DATA_BUF_CLEAN;
	return 1;
}

unsigned int FlushDataBuf(unsigned int cmdSlotTag)
{
	unsigned int dataBufEntry;
	unsigned int writebackCount = 0;

	/* Finish host DMA before taking the dirty snapshot, then wait for every
	 * scheduled NAND write before allowing the flush command to complete. */
	ReqTransSliceToLowLevel();
	SyncAllLowLevelReqDone();

	for(dataBufEntry = 0;
		dataBufEntry < AVAILABLE_DATA_BUFFER_ENTRY_COUNT;
		dataBufEntry++)
	{
		writebackCount += ScheduleDataBufWriteback(dataBufEntry, cmdSlotTag);
	}

	SyncAllLowLevelReqDone();
	return writebackCount;
}

void EvictDataBufEntry(unsigned int originReqSlotTag)
{
	unsigned int dataBufEntry;

	dataBufEntry = reqPoolPtr->reqPool[originReqSlotTag].dataBufInfo.entry;
	(void)ScheduleDataBufWriteback(dataBufEntry,
		reqPoolPtr->reqPool[originReqSlotTag].nvmeCmdSlotTag);
}

void DataReadFromNand(unsigned int originReqSlotTag)
{
	unsigned int reqSlotTag, virtualSliceAddr;

	virtualSliceAddr =  AddrTransRead(reqPoolPtr->reqPool[originReqSlotTag].logicalSliceAddr);
	//xil_printf("DataReadFromNand \r\n");
	if(virtualSliceAddr != VSA_FAIL)
	{
		reqSlotTag = GetFromFreeReqQ();

		reqPoolPtr->reqPool[reqSlotTag].reqType = REQ_TYPE_NAND;
		reqPoolPtr->reqPool[reqSlotTag].reqCode = REQ_CODE_READ;
		reqPoolPtr->reqPool[reqSlotTag].nvmeCmdSlotTag = reqPoolPtr->reqPool[originReqSlotTag].nvmeCmdSlotTag;
		reqPoolPtr->reqPool[reqSlotTag].logicalSliceAddr = reqPoolPtr->reqPool[originReqSlotTag].logicalSliceAddr;
		reqPoolPtr->reqPool[reqSlotTag].reqOpt.dataBufFormat = REQ_OPT_DATA_BUF_ENTRY;
		reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandAddr = REQ_OPT_NAND_ADDR_VSA;
		reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandEcc = REQ_OPT_NAND_ECC_ON;
		reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandEccWarning = REQ_OPT_NAND_ECC_WARNING_ON;
		reqPoolPtr->reqPool[reqSlotTag].reqOpt.rowAddrDependencyCheck = REQ_OPT_ROW_ADDR_DEPENDENCY_CHECK;
		reqPoolPtr->reqPool[reqSlotTag].reqOpt.blockSpace = REQ_OPT_BLOCK_SPACE_MAIN;

		reqPoolPtr->reqPool[reqSlotTag].dataBufInfo.entry = reqPoolPtr->reqPool[originReqSlotTag].dataBufInfo.entry;
		UpdateDataBufEntryInfoBlockingReq(reqPoolPtr->reqPool[reqSlotTag].dataBufInfo.entry, reqSlotTag);
		reqPoolPtr->reqPool[reqSlotTag].nandInfo.virtualSliceAddr = virtualSliceAddr;

		SelectLowLevelReqQ(reqSlotTag);
	}
}


void ReqTransSliceToLowLevel()
{
	unsigned int reqSlotTag, dataBufEntry;
	//print("ReqTransSliceToLowLevel \r\n");
	while(sliceReqQ.headReq != REQ_SLOT_TAG_NONE)
	{
		reqSlotTag = GetFromSliceReqQ();
		if(reqSlotTag == REQ_SLOT_TAG_FAIL)
			return ;

		//allocate a data buffer entry for this request
		dataBufEntry = CheckDataBufHit(reqSlotTag);
		// if(ADD_ONE_TAG)
		// {
		// 	xil_printf("DataBufEntry in hex: %X\r\n", dataBufEntry); 
		// 	dataBufEntry = DATA_BUF_FAIL;
		// }
		// if(FILTER_TAG)
		// {
		// 	xil_printf("DataBufEntry in hex: %X\r\n", dataBufEntry); 
		// 	//dataBufEntry = DATA_BUF_FAIL;
		// }
		if(dataBufEntry != DATA_BUF_FAIL)
		{
			//data buffer hit
			reqPoolPtr->reqPool[reqSlotTag].dataBufInfo.entry = dataBufEntry;
		}
		else
		{


			
			//data buffer miss, allocate a new buffer entry
			dataBufEntry = AllocateDataBuf();
			reqPoolPtr->reqPool[reqSlotTag].dataBufInfo.entry = dataBufEntry;

			//clear the allocated data buffer entry being used by a previous request
			EvictDataBufEntry(reqSlotTag);

			//update meta-data of the allocated data buffer entry
			dataBufMapPtr->dataBuf[dataBufEntry].logicalSliceAddr = reqPoolPtr->reqPool[reqSlotTag].logicalSliceAddr;
			PutToDataBufHashList(dataBufEntry);

			if(reqPoolPtr->reqPool[reqSlotTag].reqCode  == REQ_CODE_READ)
				DataReadFromNand(reqSlotTag);
			else if(reqPoolPtr->reqPool[reqSlotTag].reqCode  == REQ_CODE_WRITE)
				if(reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.numOfNvmeBlock != NVME_BLOCKS_PER_SLICE) //for read modify write
					DataReadFromNand(reqSlotTag);
		}

		//transform this slice request to nvme request
		if(reqPoolPtr->reqPool[reqSlotTag].reqCode  == REQ_CODE_WRITE)
		{
			dataBufMapPtr->dataBuf[dataBufEntry].dirty = DATA_BUF_DIRTY;
			reqPoolPtr->reqPool[reqSlotTag].reqCode = REQ_CODE_RxDMA;
		}
		else if(reqPoolPtr->reqPool[reqSlotTag].reqCode  == REQ_CODE_READ)
			reqPoolPtr->reqPool[reqSlotTag].reqCode = REQ_CODE_TxDMA;
		else
			assert(!"[WARNING] Not supported reqCode. [WARNING]");

		if(FILTER_TAG)
		{
			reqPoolPtr->reqPool[reqSlotTag].reqType = REQ_TYPE_NVME_FILTER;
		}
		else
		{
			reqPoolPtr->reqPool[reqSlotTag].reqType = REQ_TYPE_NVME_DMA;// FILTER_TAG
		}
		
		reqPoolPtr->reqPool[reqSlotTag].reqOpt.dataBufFormat = REQ_OPT_DATA_BUF_ENTRY;

		UpdateDataBufEntryInfoBlockingReq(dataBufEntry, reqSlotTag);// 鏇存柊缂撳啿鍖哄拰 璇锋眰鐨勯攣
		SelectLowLevelReqQ(reqSlotTag);
	}
}

unsigned int CheckBufDep(unsigned int reqSlotTag)
{
	if(reqPoolPtr->reqPool[reqSlotTag].prevBlockingReq == REQ_SLOT_TAG_NONE)// 鍓嶉潰娌℃湁鍗犵敤璇ョ紦鍐插尯鐨勮姹?
		return BUF_DEPENDENCY_REPORT_PASS;
	else
		return BUF_DEPENDENCY_REPORT_BLOCKED;
}


unsigned int CheckRowAddrDep(unsigned int reqSlotTag, unsigned int checkRowAddrDepOpt)
{
	unsigned int dieNo,chNo, wayNo, blockNo, pageNo;

	if(reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandAddr == REQ_OPT_NAND_ADDR_VSA)
	{
		dieNo = Vsa2VdieTranslation(reqPoolPtr->reqPool[reqSlotTag].nandInfo.virtualSliceAddr);
		chNo =  Vdie2PchTranslation(dieNo);
		wayNo = Vdie2PwayTranslation(dieNo);
		blockNo = Vsa2VblockTranslation(reqPoolPtr->reqPool[reqSlotTag].nandInfo.virtualSliceAddr);
		pageNo = Vsa2VpageTranslation(reqPoolPtr->reqPool[reqSlotTag].nandInfo.virtualSliceAddr);
	}
	else
		assert(!"[WARNING] Not supported reqOpt-nandAddress [WARNING]");

	if(reqPoolPtr->reqPool[reqSlotTag].reqCode == REQ_CODE_READ)
	{
		if(checkRowAddrDepOpt == ROW_ADDR_DEPENDENCY_CHECK_OPT_SELECT)
		{
			if(rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].blockedEraseReqFlag)
				SyncReleaseEraseReq(chNo, wayNo, blockNo);

			if(pageNo < rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].permittedProgPage)
				return ROW_ADDR_DEPENDENCY_REPORT_PASS;

			rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].blockedReadReqCnt++;
		}
		else if(checkRowAddrDepOpt == ROW_ADDR_DEPENDENCY_CHECK_OPT_RELEASE)
		{
			if(pageNo < rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].permittedProgPage)
			{
				rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].blockedReadReqCnt--;
				return	ROW_ADDR_DEPENDENCY_REPORT_PASS;
			}
		}
		else
			assert(!"[WARNING] Not supported checkRowAddrDepOpt [WARNING]");
	}
	else if(reqPoolPtr->reqPool[reqSlotTag].reqCode == REQ_CODE_WRITE)
	{
		if(pageNo == rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].permittedProgPage)
		{
			rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].permittedProgPage++;

			return ROW_ADDR_DEPENDENCY_REPORT_PASS;
		}
	}
	else if(reqPoolPtr->reqPool[reqSlotTag].reqCode == REQ_CODE_ERASE)
	{
		if(rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].permittedProgPage == reqPoolPtr->reqPool[reqSlotTag].nandInfo.programmedPageCnt)
			if(rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].blockedReadReqCnt == 0)
			{
				rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].permittedProgPage = 0;
				rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].blockedEraseReqFlag = 0;

				return ROW_ADDR_DEPENDENCY_REPORT_PASS;
			}

		if(checkRowAddrDepOpt == ROW_ADDR_DEPENDENCY_CHECK_OPT_SELECT)
			rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].blockedEraseReqFlag = 1;
		else if(checkRowAddrDepOpt == ROW_ADDR_DEPENDENCY_CHECK_OPT_RELEASE)
		{
			//pass, go to return
		}
		else
			assert(!"[WARNING] Not supported checkRowAddrDepOpt [WARNING]");
	}
	else
		assert(!"[WARNING] Not supported reqCode [WARNING]");

	return ROW_ADDR_DEPENDENCY_REPORT_BLOCKED;
}


unsigned int UpdateRowAddrDepTableForBufBlockedReq(unsigned int reqSlotTag)
{
	unsigned int dieNo, chNo, wayNo, blockNo, pageNo, bufDepCheckReport;

	if(reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandAddr == REQ_OPT_NAND_ADDR_VSA)
	{
		dieNo = Vsa2VdieTranslation(reqPoolPtr->reqPool[reqSlotTag].nandInfo.virtualSliceAddr);
		chNo =  Vdie2PchTranslation(dieNo);
		wayNo = Vdie2PwayTranslation(dieNo);
		blockNo = Vsa2VblockTranslation(reqPoolPtr->reqPool[reqSlotTag].nandInfo.virtualSliceAddr);
		pageNo = Vsa2VpageTranslation(reqPoolPtr->reqPool[reqSlotTag].nandInfo.virtualSliceAddr);
	}
	else
		assert(!"[WARNING] Not supported reqOpt-nandAddress [WARNING]");

	if(reqPoolPtr->reqPool[reqSlotTag].reqCode == REQ_CODE_READ)
	{
		if(rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].blockedEraseReqFlag)
		{
			SyncReleaseEraseReq(chNo, wayNo, blockNo);

			bufDepCheckReport = CheckBufDep(reqSlotTag);
			if(bufDepCheckReport == BUF_DEPENDENCY_REPORT_PASS)
			{
				if(pageNo < rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].permittedProgPage)
					PutToNandReqQ(reqSlotTag, chNo, wayNo);
				else
				{
					rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].blockedReadReqCnt++;
					PutToBlockedByRowAddrDepReqQ(reqSlotTag, chNo, wayNo);
				}

				return ROW_ADDR_DEPENDENCY_TABLE_UPDATE_REPORT_SYNC;
			}
		}
		rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].blockedReadReqCnt++;
	}
	else if(reqPoolPtr->reqPool[reqSlotTag].reqCode == REQ_CODE_ERASE)
		rowAddrDependencyTablePtr->block[chNo][wayNo][blockNo].blockedEraseReqFlag = 1;

	return ROW_ADDR_DEPENDENCY_TABLE_UPDATE_REPORT_DONE;
}



void SelectLowLevelReqQ(unsigned int reqSlotTag)
{
	unsigned int dieNo, chNo, wayNo, bufDepCheckReport, rowAddrDepCheckReport, rowAddrDepTableUpdateReport;

	bufDepCheckReport = CheckBufDep(reqSlotTag);
	// if(FILTER_TAG || ADD_ONE_TAG)
	// {	xil_printf("reqPoolPtr->reqPool[reqSlotTag].reqType %d \r\n",reqPoolPtr->reqPool[reqSlotTag].reqType);
	// 	xil_printf("SelectLowLevelReqQ \r\n");
	// 	xil_printf("bufDepCheckReport %d \r\n",bufDepCheckReport);
	// }
	
	if(bufDepCheckReport == BUF_DEPENDENCY_REPORT_PASS)
	{
		//xil_printf("FILTER_TAG %d\r\n",FILTER_TAG);
		if((ADD_ONE_TAG==0) && reqPoolPtr->reqPool[reqSlotTag].reqType  == REQ_TYPE_NVME_DMA)// 璇?鍐欒姹?
		{
			IssueNvmeDmaReq(reqSlotTag);
			PutToNvmeDmaReqQ(reqSlotTag);
		}
		else if(reqPoolPtr->reqPool[reqSlotTag].reqType  == REQ_TYPE_NVME_FILTER)
		{
			FilterQueueReadyReq(reqSlotTag);
			// 閲婃斁reqSlotTag
			//PutToNvmeDmaReqQ(reqSlotTag);
		}
		else if(ADD_ONE_TAG && reqPoolPtr->reqPool[reqSlotTag].reqType  == REQ_TYPE_NVME_DMA) // 娣诲姞鐨勫垎鏀? 闇€瑕佽ˉ鍏呯殑閫昏緫
		{
			IssueAddOne(reqSlotTag);
			PutToNvmeDmaReqQ(reqSlotTag);
		}
		else if(reqPoolPtr->reqPool[reqSlotTag].reqType  == REQ_TYPE_NAND)
		{
			if(reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandAddr == REQ_OPT_NAND_ADDR_VSA)
			{
				dieNo = Vsa2VdieTranslation(reqPoolPtr->reqPool[reqSlotTag].nandInfo.virtualSliceAddr);
				chNo =  Vdie2PchTranslation(dieNo);
				wayNo = Vdie2PwayTranslation(dieNo);
			}
			else if(reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandAddr == REQ_OPT_NAND_ADDR_PHY_ORG)
			{
				chNo =  reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalCh;
				wayNo = reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalWay;
			}
			else
				assert(!"[WARNING] Not supported reqOpt-nandAddress [WARNING]");

			if(reqPoolPtr->reqPool[reqSlotTag].reqOpt.rowAddrDependencyCheck == REQ_OPT_ROW_ADDR_DEPENDENCY_CHECK)
			{
				rowAddrDepCheckReport = CheckRowAddrDep(reqSlotTag, ROW_ADDR_DEPENDENCY_CHECK_OPT_SELECT);

				if(rowAddrDepCheckReport == ROW_ADDR_DEPENDENCY_REPORT_PASS)
					PutToNandReqQ(reqSlotTag, chNo, wayNo);
				else if(rowAddrDepCheckReport == ROW_ADDR_DEPENDENCY_REPORT_BLOCKED)
					PutToBlockedByRowAddrDepReqQ(reqSlotTag, chNo, wayNo);
				else
					assert(!"[WARNING] Not supported report [WARNING]");
			}
			else if(reqPoolPtr->reqPool[reqSlotTag].reqOpt.rowAddrDependencyCheck == REQ_OPT_ROW_ADDR_DEPENDENCY_NONE)
				PutToNandReqQ(reqSlotTag, chNo, wayNo);
			else
				assert(!"[WARNING] Not supported reqOpt [WARNING]");

		}
		else
			assert(!"[WARNING] Not supported reqType [WARNING]");
	}
	else if(bufDepCheckReport == BUF_DEPENDENCY_REPORT_BLOCKED)
	{
		if(reqPoolPtr->reqPool[reqSlotTag].reqType  == REQ_TYPE_NAND)
			if(reqPoolPtr->reqPool[reqSlotTag].reqOpt.rowAddrDependencyCheck == REQ_OPT_ROW_ADDR_DEPENDENCY_CHECK)
			{
				rowAddrDepTableUpdateReport = UpdateRowAddrDepTableForBufBlockedReq(reqSlotTag);

				if(rowAddrDepTableUpdateReport == ROW_ADDR_DEPENDENCY_TABLE_UPDATE_REPORT_DONE)
				{
					//pass, go to PutToBlockedByBufDepReqQ
				}
				else if(rowAddrDepTableUpdateReport == ROW_ADDR_DEPENDENCY_TABLE_UPDATE_REPORT_SYNC)
					return;
				else
					assert(!"[WARNING] Not supported report [WARNING]");
			}

		PutToBlockedByBufDepReqQ(reqSlotTag);
	}
	else
		assert(!"[WARNING] Not supported report [WARNING]");
}


void ReleaseBlockedByBufDepReq(unsigned int reqSlotTag)
{
	unsigned int targetReqSlotTag, dieNo, chNo, wayNo, rowAddrDepCheckReport;

	targetReqSlotTag = REQ_SLOT_TAG_NONE;
	if(reqPoolPtr->reqPool[reqSlotTag].nextBlockingReq != REQ_SLOT_TAG_NONE)
	{
		targetReqSlotTag = reqPoolPtr->reqPool[reqSlotTag].nextBlockingReq;
		reqPoolPtr->reqPool[targetReqSlotTag].prevBlockingReq = REQ_SLOT_TAG_NONE;
		reqPoolPtr->reqPool[reqSlotTag].nextBlockingReq = REQ_SLOT_TAG_NONE;
	}

	if(reqPoolPtr->reqPool[reqSlotTag].reqOpt.dataBufFormat == REQ_OPT_DATA_BUF_ENTRY)
	{
		if(dataBufMapPtr->dataBuf[reqPoolPtr->reqPool[reqSlotTag].dataBufInfo.entry].blockingReqTail == reqSlotTag)
			dataBufMapPtr->dataBuf[reqPoolPtr->reqPool[reqSlotTag].dataBufInfo.entry].blockingReqTail = REQ_SLOT_TAG_NONE;
	}
	else if(reqPoolPtr->reqPool[reqSlotTag].reqOpt.dataBufFormat == REQ_OPT_DATA_BUF_TEMP_ENTRY)
	{
		if(tempDataBufMapPtr->tempDataBuf[reqPoolPtr->reqPool[reqSlotTag].dataBufInfo.entry].blockingReqTail == reqSlotTag)
			tempDataBufMapPtr->tempDataBuf[reqPoolPtr->reqPool[reqSlotTag].dataBufInfo.entry].blockingReqTail = REQ_SLOT_TAG_NONE;
	}

	if((targetReqSlotTag != REQ_SLOT_TAG_NONE) && (reqPoolPtr->reqPool[targetReqSlotTag].reqQueueType == REQ_QUEUE_TYPE_BLOCKED_BY_BUF_DEP))
	{
		SelectiveGetFromBlockedByBufDepReqQ(targetReqSlotTag);
		//xil_printf("ReleaseBlockedByBufDepReq reqPoolPtr->reqPool[targetReqSlotTag].reqType %d  FILTER_TAG %d",reqPoolPtr->reqPool[targetReqSlotTag].reqType, FILTER_TAG);
		if(reqPoolPtr->reqPool[targetReqSlotTag].reqType == REQ_TYPE_NVME_DMA)
		{
			IssueNvmeDmaReq(targetReqSlotTag);
			PutToNvmeDmaReqQ(targetReqSlotTag);
		}
		else if(reqPoolPtr->reqPool[targetReqSlotTag].reqType  == REQ_TYPE_NVME_FILTER)
		{
			FilterQueueReadyReq(targetReqSlotTag);
            // 閲婃斁reqSlotTag
		}
		else if(reqPoolPtr->reqPool[targetReqSlotTag].reqType  == REQ_TYPE_NAND)
		{
			if(reqPoolPtr->reqPool[targetReqSlotTag].reqOpt.nandAddr == REQ_OPT_NAND_ADDR_VSA)
			{
				dieNo = Vsa2VdieTranslation(reqPoolPtr->reqPool[targetReqSlotTag].nandInfo.virtualSliceAddr);
				chNo =  Vdie2PchTranslation(dieNo);
				wayNo = Vdie2PwayTranslation(dieNo);
			}
			else
				assert(!"[WARNING] Not supported reqOpt-nandAddress [WARNING]");

			if(reqPoolPtr->reqPool[targetReqSlotTag].reqOpt.rowAddrDependencyCheck == REQ_OPT_ROW_ADDR_DEPENDENCY_CHECK)
			{
				rowAddrDepCheckReport = CheckRowAddrDep(targetReqSlotTag, ROW_ADDR_DEPENDENCY_CHECK_OPT_RELEASE);

				if(rowAddrDepCheckReport == ROW_ADDR_DEPENDENCY_REPORT_PASS)
					PutToNandReqQ(targetReqSlotTag, chNo, wayNo);
				else if(rowAddrDepCheckReport == ROW_ADDR_DEPENDENCY_REPORT_BLOCKED)
					PutToBlockedByRowAddrDepReqQ(targetReqSlotTag, chNo, wayNo);
				else
					assert(!"[WARNING] Not supported report [WARNING]");
			}
			else if(reqPoolPtr->reqPool[targetReqSlotTag].reqOpt.rowAddrDependencyCheck == REQ_OPT_ROW_ADDR_DEPENDENCY_NONE)
				PutToNandReqQ(targetReqSlotTag, chNo, wayNo);
			else
				assert(!"[WARNING] Not supported reqOpt [WARNING]");
		}
	}
}


void ReleaseBlockedByRowAddrDepReq(unsigned int chNo, unsigned int wayNo)
{
	unsigned int reqSlotTag, nextReq, rowAddrDepCheckReport;

	reqSlotTag = blockedByRowAddrDepReqQ[chNo][wayNo].headReq;

	while(reqSlotTag != REQ_SLOT_TAG_NONE)
	{
		nextReq = reqPoolPtr->reqPool[reqSlotTag].nextReq;

		if(reqPoolPtr->reqPool[reqSlotTag].reqOpt.rowAddrDependencyCheck == REQ_OPT_ROW_ADDR_DEPENDENCY_CHECK)
		{
			rowAddrDepCheckReport = CheckRowAddrDep(reqSlotTag, ROW_ADDR_DEPENDENCY_CHECK_OPT_RELEASE);

			if(rowAddrDepCheckReport == ROW_ADDR_DEPENDENCY_REPORT_PASS)
			{
				SelectiveGetFromBlockedByRowAddrDepReqQ(reqSlotTag, chNo, wayNo);
				PutToNandReqQ(reqSlotTag, chNo, wayNo);
			}
			else if(rowAddrDepCheckReport == ROW_ADDR_DEPENDENCY_REPORT_BLOCKED)
			{
				//pass, go to while loop
			}
			else
				assert(!"[WARNING] Not supported report [WARNING]");
		}
		else
			assert(!"[WARNING] Not supported reqOpt [WARNING]");

		reqSlotTag = nextReq;
	}
}
void print_devAddr_values(unsigned int *devAddr) {
    // 璇诲彇骞舵墦鍗板墠 10 涓暟瀛?
	int num_items = 10;
    for (int i = 0; i < num_items; i++) {
        xil_printf("devAddr[%d] = %d\r\n", i, devAddr[i]);
    }
}
void AddOne_devAddr_values(unsigned int *devAddr) {
    // 鎵€鏈夋暟瀛?1
    for (int i = 0; i < BYTES_PER_NVME_BLOCK / 4; i++) 
	{
		devAddr[i] += 1;
        xil_printf("devAddr[%d] = %d\r\n", i, devAddr[i]);
    }
}
void IssueAddOne(unsigned int reqSlotTag)
{
	unsigned int devAddr, dmaIndex, numOfNvmeBlock;

	dmaIndex = reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.startIndex;// 鐞嗚ВstartIndex鐨勫惈涔?
	devAddr = GenerateDataBufAddr(reqSlotTag);
	//print_devAddr_values(devAddr);
	numOfNvmeBlock = 0;
	if(reqPoolPtr->reqPool[reqSlotTag].reqCode == REQ_CODE_TxDMA)
	{
		while(numOfNvmeBlock < reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.numOfNvmeBlock)
		{
			//AddOne_devAddr_values(devAddr);
			set_auto_tx_dma(reqPoolPtr->reqPool[reqSlotTag].nvmeCmdSlotTag, dmaIndex, devAddr, NVME_COMMAND_AUTO_COMPLETION_ON);

			numOfNvmeBlock++;
			dmaIndex++;
			devAddr += BYTES_PER_NVME_BLOCK;
		}
		reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.reqTail =  g_hostDmaStatus.fifoTail.autoDmaTx;
		reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.overFlowCnt = g_hostDmaAssistStatus.autoDmaTxOverFlowCnt;
	}
	else
	{
		xil_printf("[WARNING] Not supported reqCode [WARNING]\r\n");
	}

}
static void filter_materialize_spans(unsigned char *page_data,
									 uint16_t pd_lower,
									 const unsigned short *tuple_offs,
									 const unsigned short *tuple_lens,
									 int hit_count,
									 int *result_full)
{
	uint64_t t0 = FilterProfileNow();

	if(hit_count > MAX_TUPLE_PER_PAGE)
		hit_count = MAX_TUPLE_PER_PAGE;

	if(filter_count_only_mode)
	{
		if(hit_count > 0)
			filter_count_only_hits += (uint64_t)hit_count;
		FilterProfileAddMaterializeCycles(FilterProfileNow() - t0);
		return;
	}

	if(filter_compute_enabled())
	{
		(void)filter_compute_consume_arm(page_data, tuple_offs, tuple_lens,
									 hit_count, csd_tupdesc,
									 (uint32_t)csd_qual->natts, pd_lower);
		FilterProfileAddMaterializeCycles(FilterProfileNow() - t0);
		return;
	}

	if(filter_join_enabled())
	{
		(void)filter_join_consume_arm(page_data, tuple_offs, tuple_lens,
								  hit_count, csd_tupdesc,
								  (uint32_t)csd_qual->natts, pd_lower);
		FilterProfileAddMaterializeCycles(FilterProfileNow() - t0);
		return;
	}

	if(filter_projection_enabled())
	{
		uint32_t rows_written = 0;

		if(filter_projection_append_arm(page_data, tuple_offs, tuple_lens,
									 hit_count, csd_tupdesc,
									 (uint32_t)csd_qual->natts,
									 pd_lower, &rows_written) < 0)
		{
			*result_full = 1;
			filter_exec_stats.result_full_events++;
			if(hit_count > (int)rows_written)
				filter_exec_stats.result_dropped_tuples +=
					(uint64_t)(hit_count - (int)rows_written);
		}
		FilterProfileAddMaterializeCycles(FilterProfileNow() - t0);
		return;
	}

	for(int i = 0; i < hit_count; i++)
	{
		HeapTupleData tuple;
		unsigned short tup_off = tuple_offs[i];
		unsigned short tup_len = tuple_lens[i];

		if(tup_off < pd_lower ||
		   tup_len == 0 ||
		   (tup_off + tup_len) > PG_PAGE_SIZE)
		{
			continue;
		}

		tuple.t_data = (char *)(page_data + tup_off);
		tuple.t_len = tup_len;
		if(append_tuple_to_mem_page(&writer, &tuple) < 0)
		{
			*result_full = 1;
			filter_exec_stats.result_full_events++;
			filter_exec_stats.result_dropped_tuples += (uint64_t)(hit_count - i);
			break;
		}
	}

	int result_page_num = writer.cur_page_index + 1;
	memcpy((void *)(uintptr_t)FILTER_RESULT_DRAM_DATA_BUFFER,
		   &result_page_num, sizeof(result_page_num));
	FilterProfileAddMaterializeCycles(FilterProfileNow() - t0);
}

static void filter_materialize_fpga_or_arm(unsigned char *page_data,
										   uint16_t pd_lower,
										   const unsigned short *tuple_offs,
										   const unsigned short *tuple_lens,
										   int hit_count,
										   unsigned int staging_page,
										   int *result_full)
{
	if(filter_compute_fpga_enabled())
	{
		uint64_t t0 = FilterProfileNow();
		const void *staging = (const void *)(uintptr_t)
			(FILTER_COMPUTE_DRAM_DATA_BUFFER +
			 staging_page * FILTER_PROJECTION_STAGING_BYTES_PER_PAGE);

		if(filter_compute_merge_fpga(
				staging, FILTER_PROJECTION_STAGING_BYTES_PER_PAGE,
				(uint32_t)((hit_count > 0) ? hit_count : 0)) == 0)
		{
			FilterProfileAddMaterializeCycles(FilterProfileNow() - t0);
			return;
		}
		FilterProfileAddMaterializeCycles(FilterProfileNow() - t0);
		filter_exec_stats.fpga_discard_pages++;
		filter_exec_stats.fallback_pages++;
	}

	if(filter_join_fpga_enabled())
	{
		uint64_t t0 = FilterProfileNow();
		const void *staging = (const void *)(uintptr_t)
			(FILTER_COMPUTE_DRAM_DATA_BUFFER +
			 staging_page * FILTER_PROJECTION_STAGING_BYTES_PER_PAGE);

		if(filter_join_consume_fpga_hashes(
				staging, FILTER_PROJECTION_STAGING_BYTES_PER_PAGE,
				page_data, tuple_offs, tuple_lens, hit_count, csd_tupdesc,
				(uint32_t)csd_qual->natts, pd_lower) == 0)
		{
			FilterProfileAddMaterializeCycles(FilterProfileNow() - t0);
			return;
		}
		FilterProfileAddMaterializeCycles(FilterProfileNow() - t0);
		filter_exec_stats.fpga_discard_pages++;
		filter_exec_stats.fallback_pages++;
	}

	if(filter_projection_mode() == FILTER_PROJECTION_MODE_FPGA)
	{
		uint64_t t0 = FilterProfileNow();
		const void *staging = (const void *)(uintptr_t)
			(FILTER_PROJECTION_DRAM_DATA_BUFFER +
			 staging_page * FILTER_PROJECTION_STAGING_BYTES_PER_PAGE);
		int projection_rc = filter_projection_append_fpga(
			staging, FILTER_PROJECTION_STAGING_BYTES_PER_PAGE,
			(uint32_t)((hit_count > 0) ? hit_count : 0));

		FilterProfileAddMaterializeCycles(FilterProfileNow() - t0);
		if(projection_rc == 0)
			return;
		if(projection_rc == -1)
		{
			*result_full = 1;
			filter_exec_stats.result_full_events++;
			filter_exec_stats.result_dropped_tuples +=
				(uint64_t)((hit_count > 0) ? hit_count : 0);
			return;
		}
		filter_exec_stats.fpga_discard_pages++;
		filter_exec_stats.fallback_pages++;
	}

	filter_materialize_spans(page_data, pd_lower, tuple_offs, tuple_lens,
							 hit_count, result_full);
}

static int filter_collect_fpga_spans(const unsigned int *hit_desc,
									 int hit_count,
									 unsigned short *tuple_offs,
									 unsigned short *tuple_lens)
{
	if(hit_count < 0)
		hit_count = 0;
	if(hit_count > MAX_TUPLE_PER_PAGE)
		hit_count = MAX_TUPLE_PER_PAGE;
	if(filter_count_only_mode)
		return hit_count;

	for(int i = 0; i < hit_count; i++)
	{
		unsigned int desc = hit_desc[i];
		tuple_offs[i] = FILTER_ACCEL_HIT_TUPLE_OFF(desc);
		tuple_lens[i] = FILTER_ACCEL_HIT_TUPLE_LEN(desc);
	}

	return hit_count;
}

static int filter_collect_arm_spans(unsigned char *page_data,
									int page_id,
									int tuple_cnt,
									int natts,
									int filter_cnt,
									unsigned short *tuple_offs,
									unsigned short *tuple_lens)
{
	int hit_count_out = 0;
	int out_count = 0;

	get_tuple_attr_pos(page_data, 0, page_id, natts,
					   csd_tupdesc->desc_len_in, csd_tupdesc->desc_align_in,
					   tuple_cnt,
					   filter_tup_off_array, filter_tup_len_array,
					   filter_pos_array, filter_len_array);
	filter_tuple(page_data, 0, page_id, natts, filter_cnt, csd_filter,
				 tuple_cnt, filter_tup_off_array, filter_tup_len_array,
				 filter_pos_array, filter_len_array,
				 filter_hit_off, &hit_count_out);

	if(hit_count_out > MAX_TUPLE_PER_PAGE)
		hit_count_out = MAX_TUPLE_PER_PAGE;

	for(int i = 0; i < hit_count_out; i++)
	{
		int index = filter_hit_off[i] & 0xffff;
		if(index >= 0 && index < tuple_cnt &&
		   filter_tup_off_array[index] != 0 &&
		   filter_tup_len_array[index] != 0 &&
		   (filter_tup_off_array[index] + filter_tup_len_array[index]) <= PG_PAGE_SIZE)
		{
			tuple_offs[out_count] = filter_tup_off_array[index];
			tuple_lens[out_count] = filter_tup_len_array[index];
			out_count++;
		}
	}

	return out_count;
}

#if FILTER_ACCEL_VERIFY_ENABLE
static unsigned int filter_compare_spans(uint16_t pd_lower,
										 const unsigned short *fpga_offs,
										 const unsigned short *fpga_lens,
										 int fpga_count,
										 const unsigned short *arm_offs,
										 const unsigned short *arm_lens,
										 int arm_count)
{
	unsigned int mismatch = 0;
	int min_count = (fpga_count < arm_count) ? fpga_count : arm_count;

	/* Count-only FPGA results intentionally contain no tuple descriptors. */
	if(filter_count_only_mode)
		return (fpga_count > arm_count) ? (unsigned int)(fpga_count - arm_count)
										: (unsigned int)(arm_count - fpga_count);

	if(fpga_count != arm_count)
		mismatch += (fpga_count > arm_count) ? (unsigned int)(fpga_count - arm_count)
											 : (unsigned int)(arm_count - fpga_count);

	for(int i = 0; i < min_count; i++)
	{
		int fpga_invalid = (fpga_offs[i] < pd_lower ||
							fpga_lens[i] == 0 ||
							(fpga_offs[i] + fpga_lens[i]) > PG_PAGE_SIZE);
		if(fpga_invalid ||
		   fpga_offs[i] != arm_offs[i] ||
		   fpga_lens[i] != arm_lens[i])
		{
			mismatch++;
		}
	}

	return mismatch;
}
#endif

static void filter_run_arm_page(unsigned char *page_data,
								uint16_t pd_lower,
								int page_id,
								int tuple_cnt,
								int natts,
								int filter_cnt,
								int *result_full)
{
	uint64_t t0 = FilterProfileNow();
	int hit_count_out = filter_collect_arm_spans(page_data,
												 page_id,
												 tuple_cnt,
												 natts,
												 filter_cnt,
												 filter_hit_tuple_off,
												 filter_hit_tuple_len);
	FilterProfileAddArmFallbackCycles(FilterProfileNow() - t0);

	filter_materialize_spans(page_data, pd_lower,
							 filter_hit_tuple_off,
							 filter_hit_tuple_len,
							 hit_count_out,
							 result_full);
}

void FilterFlushPendingBatch(void)
{
#if FILTER_ACCEL_ENABLE && FILTER_STAGING_BATCH_ENABLE
	unsigned int pending_pages = filter_batch_pending_pages;
	int result_full = (int)filter_batch_result_full;

	if(pending_pages == 0)
		return;

	filter_exec_stats.filter_flush_calls++;
	filter_exec_stats.filter_flush_pages += pending_pages;
	if(filter_nand_read_pending > 0)
		filter_exec_stats.filter_flush_while_nand_pending++;

	filter_batch_pending_pages = 0;
	if(filter_force_arm_fallback_enabled())
		return;
	if(result_full)
		return;

	uint64_t t0 = FilterProfileNow();
	int accel_status = filter_accel_run_batch(FILTER_BATCH_DRAM_DATA_BUFFER,
											  FILTER_INFO_DRAM_DATA_BUFFER,
											  FILTER_INFO_DRAM_DATA_BUFFER + 0x1000,
											  FILTER_INFO_DRAM_DATA_BUFFER + 0x2000,
											  FILTER_HIT_DRAM_DATA_BUFFER,
											  0,
											  csd_qual->natts,
											  csd_qual->filter_cnt,
											  filter_batch_tuple_len,
											  pending_pages,
											  PG_PAGE_SIZE,
											  FILTER_ACCEL_BATCH_HIT_STRIDE);
	FilterProfileAddFpgaComputeCycles(FilterProfileNow() - t0);

	if(accel_status == FILTER_ACCEL_OK)
	{
		unsigned int *hit_count_table =
			(unsigned int *)(uintptr_t)FILTER_HIT_DRAM_DATA_BUFFER;
		filter_exec_stats.batch_invocations++;
		filter_exec_stats.batch_pages += pending_pages;
		for(unsigned int page_id = 0; page_id < pending_pages && !result_full; page_id++)
		{
			unsigned char *page_data = (unsigned char *)(uintptr_t)
				(FILTER_BATCH_DRAM_DATA_BUFFER + page_id * PG_PAGE_SIZE);
			unsigned int *hit_desc = (unsigned int *)(uintptr_t)(FILTER_HIT_DRAM_DATA_BUFFER +
											 FILTER_ACCEL_BATCH_DESC_OFFSET +
											 page_id * FILTER_ACCEL_BATCH_HIT_STRIDE);
			int fpga_hit_count = filter_collect_fpga_spans(hit_desc,
														   (int)hit_count_table[page_id],
														   filter_fpga_tuple_off,
														   filter_fpga_tuple_len);
#if FILTER_ACCEL_VERIFY_ENABLE
			uint64_t verify_t0 = FilterProfileNow();
			int arm_hit_count = filter_collect_arm_spans(page_data,
														 (int)page_id,
														 filter_batch_tuple_len[page_id],
														 csd_qual->natts,
														 csd_qual->filter_cnt,
														 filter_arm_tuple_off,
														 filter_arm_tuple_len);
			unsigned int mismatch = filter_compare_spans(filter_batch_pd_lower[page_id],
														 filter_fpga_tuple_off,
														 filter_fpga_tuple_len,
														 fpga_hit_count,
														 filter_arm_tuple_off,
														 filter_arm_tuple_len,
														 arm_hit_count);
			uint64_t verify_cycles = FilterProfileNow() - verify_t0;
			FilterProfileAddVerifyCycles(verify_cycles);
			filter_exec_stats.verify_pages++;
			if(mismatch != 0)
			{
				filter_exec_stats.verify_mismatch_pages++;
				filter_exec_stats.verify_mismatch_tuples += mismatch;
				filter_exec_stats.fpga_discard_pages++;
				filter_exec_stats.fallback_pages++;
				FilterProfileAddArmFallbackCycles(verify_cycles);
				filter_materialize_spans(page_data,
										 filter_batch_pd_lower[page_id],
										 filter_arm_tuple_off,
										 filter_arm_tuple_len,
										 arm_hit_count,
										 &result_full);
			}
			else
#endif
			{
				filter_exec_stats.fpga_pages++;
				filter_materialize_fpga_or_arm(page_data,
										 filter_batch_pd_lower[page_id],
										 filter_fpga_tuple_off,
										 filter_fpga_tuple_len,
										 fpga_hit_count,
										 page_id,
										 &result_full);
			}
		}
	}
	else
	{
		filter_exec_stats.last_accel_status = accel_status;
		filter_exec_stats.last_hw_status = filter_accel_read_status();
		filter_exec_stats.last_hw_error = filter_accel_read_error();
		for(unsigned int page_id = 0; page_id < pending_pages && !result_full; page_id++)
		{
			unsigned char *page_data = (unsigned char *)(uintptr_t)
				(FILTER_BATCH_DRAM_DATA_BUFFER + page_id * PG_PAGE_SIZE);
			filter_exec_stats.fallback_pages++;
			filter_run_arm_page(page_data,
								filter_batch_pd_lower[page_id],
								(int)page_id,
								filter_batch_tuple_len[page_id],
								csd_qual->natts,
								csd_qual->filter_cnt,
								&result_full);
		}
	}

	filter_batch_result_full = (unsigned int)result_full;
#endif
}

#if FILTER_ACCEL_ENABLE && FILTER_STAGING_BATCH_ENABLE
static int filter_try_stage_batch(unsigned int devAddr,
								  unsigned int page_count)
{
	if(filter_batch_result_full)
		return 1;

	if(page_count == 0 || page_count > (BYTES_PER_DATA_REGION_OF_SLICE / PG_PAGE_SIZE))
		return 0;

	for(unsigned int page_id = 0; page_id < page_count; page_id++)
	{
		unsigned int page_devAddr = devAddr + page_id * PG_PAGE_SIZE;
		uint8_t *base = (uint8_t *)(uintptr_t)page_devAddr;
		uint16_t pd_lower = ((uint16_t)base[13] << 8) | base[12];

		filter_page_tuple_off[page_id] = pd_lower;
		filter_page_tuple_len[page_id] = 0;

		if(pd_lower < 24 || pd_lower > PG_PAGE_SIZE || ((pd_lower - 24) & 0x3))
			return 0;

		unsigned int tuple_cnt = (pd_lower - 24) >> 2;
		if(tuple_cnt > MAX_TUPLE_PER_PAGE)
			tuple_cnt = MAX_TUPLE_PER_PAGE;
		if(tuple_cnt > FILTER_ACCEL_MAX_TUPLES_PER_PAGE)
			return 0;

		filter_page_tuple_len[page_id] = (unsigned short)tuple_cnt;
	}

	for(unsigned int page_id = 0; page_id < page_count; page_id++)
	{
		if(filter_batch_pending_pages >= FILTER_STAGING_BATCH_PAGES)
			FilterFlushPendingBatch();
		if(filter_batch_result_full)
			return 1;

		unsigned int dst = FILTER_BATCH_DRAM_DATA_BUFFER + filter_batch_pending_pages * PG_PAGE_SIZE;
		unsigned int src = devAddr + page_id * PG_PAGE_SIZE;
		uint64_t copy_t0 = FilterProfileNow();
		memcpy((void *)(uintptr_t)dst, (const void *)(uintptr_t)src, PG_PAGE_SIZE);
		FilterProfileAddStagingCopyCycles(FilterProfileNow() - copy_t0);
		filter_batch_pd_lower[filter_batch_pending_pages] = filter_page_tuple_off[page_id];
		filter_batch_tuple_len[filter_batch_pending_pages] = filter_page_tuple_len[page_id];
		filter_batch_pending_pages++;
		filter_exec_stats.total_pages++;
	}

	filter_exec_stats.filter_stage_calls++;
	filter_exec_stats.filter_stage_pages += page_count;

	if(filter_batch_pending_pages >= FILTER_STAGING_BATCH_PAGES)
		FilterFlushPendingBatch();

	return 1;
}
#endif

void IssueFilter(unsigned int reqSlotTag)
{
	FILTER_TRACE("IssueFilter\r\n");
	filter_exec_stats.filter_issue_calls++;
	unsigned int devAddr, numOfNvmeBlock;
	int result_full = 0;
	int natts = csd_qual->natts;
	int filter_cnt = csd_qual->filter_cnt;
	unsigned int page_count;
	unsigned int valid_page_count = 0;
	unsigned int can_batch = 1;

	devAddr = GenerateDataBufAddr(reqSlotTag);
	numOfNvmeBlock = reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.numOfNvmeBlock;

	if(numOfNvmeBlock != 4 && numOfNvmeBlock != 2)
	{
		filter_exec_stats.last_accel_status = -200;
		return;
	}

	if(reqPoolPtr->reqPool[reqSlotTag].reqCode != REQ_CODE_TxDMA)
	{
		filter_exec_stats.last_accel_status = -201;
		return;
	}

	page_count = numOfNvmeBlock / (PG_PAGE_SIZE / BYTES_PER_NVME_BLOCK);
	if(page_count > (BYTES_PER_DATA_REGION_OF_SLICE / PG_PAGE_SIZE))
		page_count = BYTES_PER_DATA_REGION_OF_SLICE / PG_PAGE_SIZE;

#if FILTER_ACCEL_ENABLE && FILTER_STAGING_BATCH_ENABLE
	if(!filter_force_arm_fallback_enabled() && filter_try_stage_batch(devAddr, page_count))
		return;
	FilterFlushPendingBatch();
#endif

	for(unsigned int page_id = 0; page_id < page_count; page_id++)
	{
		unsigned int page_devAddr = devAddr + page_id * PG_PAGE_SIZE;
		uint8_t *base = (uint8_t *)(uintptr_t)page_devAddr;
		uint16_t pd_lower = ((uint16_t)base[13] << 8) | base[12];

		filter_page_tuple_off[page_id] = pd_lower;
		filter_page_tuple_len[page_id] = 0;

		if(pd_lower < 24 || pd_lower > PG_PAGE_SIZE || ((pd_lower - 24) & 0x3))
		{
			filter_exec_stats.invalid_pages++;
			can_batch = 0;
			continue;
		}

		filter_exec_stats.total_pages++;
		valid_page_count++;
		unsigned int tuple_cnt = (pd_lower - 24) >> 2;
		if(tuple_cnt > MAX_TUPLE_PER_PAGE)
			tuple_cnt = MAX_TUPLE_PER_PAGE;
		filter_page_tuple_len[page_id] = (unsigned short)tuple_cnt;
		if(tuple_cnt > FILTER_ACCEL_MAX_TUPLES_PER_PAGE)
			can_batch = 0;
	}

#if FILTER_ACCEL_ENABLE
	if(!filter_force_arm_fallback_enabled() && can_batch && valid_page_count == page_count && page_count > 0)
	{
		uint64_t t0 = FilterProfileNow();
		int accel_status = filter_accel_run_batch(devAddr,
												  FILTER_INFO_DRAM_DATA_BUFFER,
												  FILTER_INFO_DRAM_DATA_BUFFER + 0x1000,
												  FILTER_INFO_DRAM_DATA_BUFFER + 0x2000,
												  FILTER_HIT_DRAM_DATA_BUFFER,
												  0,
												  natts,
												  filter_cnt,
												  filter_page_tuple_len,
												  page_count,
												  PG_PAGE_SIZE,
												  FILTER_ACCEL_BATCH_HIT_STRIDE);
		FilterProfileAddFpgaComputeCycles(FilterProfileNow() - t0);
		if(accel_status == FILTER_ACCEL_OK)
		{
			unsigned int *hit_count_table =
				(unsigned int *)(uintptr_t)FILTER_HIT_DRAM_DATA_BUFFER;
			filter_exec_stats.batch_invocations++;
			filter_exec_stats.batch_pages += page_count;
			for(unsigned int page_id = 0; page_id < page_count && !result_full; page_id++)
			{
				unsigned char *page_data = (unsigned char *)(uintptr_t)
					(devAddr + page_id * PG_PAGE_SIZE);
				unsigned int *hit_desc = (unsigned int *)(uintptr_t)(FILTER_HIT_DRAM_DATA_BUFFER +
												 FILTER_ACCEL_BATCH_DESC_OFFSET +
												 page_id * FILTER_ACCEL_BATCH_HIT_STRIDE);
				int fpga_hit_count = filter_collect_fpga_spans(hit_desc,
															   (int)hit_count_table[page_id],
															   filter_fpga_tuple_off,
															   filter_fpga_tuple_len);
#if FILTER_ACCEL_VERIFY_ENABLE
				uint64_t verify_t0 = FilterProfileNow();
				int arm_hit_count = filter_collect_arm_spans(page_data,
															 page_id,
															 filter_page_tuple_len[page_id],
															 natts,
															 filter_cnt,
															 filter_arm_tuple_off,
															 filter_arm_tuple_len);
				unsigned int mismatch = filter_compare_spans(filter_page_tuple_off[page_id],
															 filter_fpga_tuple_off,
															 filter_fpga_tuple_len,
															 fpga_hit_count,
															 filter_arm_tuple_off,
															 filter_arm_tuple_len,
															 arm_hit_count);
				uint64_t verify_cycles = FilterProfileNow() - verify_t0;
				FilterProfileAddVerifyCycles(verify_cycles);
				filter_exec_stats.verify_pages++;
				if(mismatch != 0)
				{
					filter_exec_stats.verify_mismatch_pages++;
					filter_exec_stats.verify_mismatch_tuples += mismatch;
					filter_exec_stats.fpga_discard_pages++;
					filter_exec_stats.fallback_pages++;
					FilterProfileAddArmFallbackCycles(verify_cycles);
					filter_materialize_spans(page_data,
											 filter_page_tuple_off[page_id],
											 filter_arm_tuple_off,
											 filter_arm_tuple_len,
											 arm_hit_count,
											 &result_full);
				}
				else
#endif
				{
					filter_exec_stats.fpga_pages++;
					filter_materialize_fpga_or_arm(page_data,
											 filter_page_tuple_off[page_id],
											 filter_fpga_tuple_off,
											 filter_fpga_tuple_len,
											 fpga_hit_count,
											 page_id,
											 &result_full);
				}
			}
			return;
		}

		filter_exec_stats.last_accel_status = accel_status;
		filter_exec_stats.last_hw_status = filter_accel_read_status();
		filter_exec_stats.last_hw_error = filter_accel_read_error();
	}
#endif

	for(unsigned int page_id = 0; page_id < page_count && !result_full; page_id++)
	{
		if(filter_page_tuple_len[page_id] == 0)
			continue;

		unsigned int page_devAddr = devAddr + page_id * PG_PAGE_SIZE;
		unsigned char *page_data = (unsigned char *)(uintptr_t)page_devAddr;
		int tuple_cnt = filter_page_tuple_len[page_id];
		int use_arm_filter = 1;

#if FILTER_ACCEL_ENABLE
		if(!filter_force_arm_fallback_enabled() &&
		   tuple_cnt <= FILTER_ACCEL_MAX_TUPLES_PER_PAGE)
		{
			uint32_t accel_hit_count = 0;
			uint64_t t0 = FilterProfileNow();
			int accel_status = filter_accel_run_page(page_devAddr,
													 FILTER_INFO_DRAM_DATA_BUFFER,
													 FILTER_INFO_DRAM_DATA_BUFFER + 0x1000,
													 FILTER_INFO_DRAM_DATA_BUFFER + 0x2000,
													 FILTER_HIT_DRAM_DATA_BUFFER,
													 page_id,
													 natts,
													 filter_cnt,
													 tuple_cnt,
													 &accel_hit_count);
			FilterProfileAddFpgaComputeCycles(FilterProfileNow() - t0);
			if(accel_status == FILTER_ACCEL_OK)
			{
				unsigned int *hit_desc =
					(unsigned int *)(uintptr_t)FILTER_HIT_DRAM_DATA_BUFFER;
				int fpga_hit_count = filter_collect_fpga_spans(hit_desc,
															   (int)accel_hit_count,
															   filter_fpga_tuple_off,
															   filter_fpga_tuple_len);
#if FILTER_ACCEL_VERIFY_ENABLE
				uint64_t verify_t0 = FilterProfileNow();
				int arm_hit_count = filter_collect_arm_spans(page_data,
															 page_id,
															 tuple_cnt,
															 natts,
															 filter_cnt,
															 filter_arm_tuple_off,
															 filter_arm_tuple_len);
				unsigned int mismatch = filter_compare_spans(filter_page_tuple_off[page_id],
															 filter_fpga_tuple_off,
															 filter_fpga_tuple_len,
															 fpga_hit_count,
															 filter_arm_tuple_off,
															 filter_arm_tuple_len,
															 arm_hit_count);
				uint64_t verify_cycles = FilterProfileNow() - verify_t0;
				FilterProfileAddVerifyCycles(verify_cycles);
				filter_exec_stats.verify_pages++;
				if(mismatch != 0)
				{
					filter_exec_stats.verify_mismatch_pages++;
					filter_exec_stats.verify_mismatch_tuples += mismatch;
					filter_exec_stats.fpga_discard_pages++;
					filter_exec_stats.fallback_pages++;
					FilterProfileAddArmFallbackCycles(verify_cycles);
					filter_materialize_spans(page_data,
											 filter_page_tuple_off[page_id],
											 filter_arm_tuple_off,
											 filter_arm_tuple_len,
											 arm_hit_count,
											 &result_full);
				}
				else
#endif
				{
					filter_exec_stats.fpga_pages++;
					filter_materialize_fpga_or_arm(page_data,
											 filter_page_tuple_off[page_id],
											 filter_fpga_tuple_off,
											 filter_fpga_tuple_len,
											 fpga_hit_count,
											 0,
											 &result_full);
				}
				use_arm_filter = 0;
			}
			else
			{
				filter_exec_stats.last_accel_status = accel_status;
				filter_exec_stats.last_hw_status = filter_accel_read_status();
				filter_exec_stats.last_hw_error = filter_accel_read_error();
			}
		}
		else
		{
			filter_exec_stats.last_accel_status = filter_force_arm_fallback_enabled() ? -102 : -100;
		}
#else
		filter_exec_stats.last_accel_status = -101;
#endif

		if(use_arm_filter)
		{
			filter_exec_stats.fallback_pages++;
			filter_run_arm_page(page_data,
								filter_page_tuple_off[page_id],
								page_id,
								tuple_cnt,
								natts,
								filter_cnt,
								&result_full);
		}
	}
}

#if FILTER_ACCEL_ENABLE && FILTER_DIRECT_COALESCE_ENABLE
static int filter_prepare_direct_coalesce_req(unsigned int reqSlotTag,
											  unsigned int *devAddr_out,
											  unsigned int *page_count_out,
											  unsigned int *page_addr_out,
											  unsigned short *pd_lower_out,
											  unsigned short *tuple_len_out,
											  unsigned int max_pages)
{
	unsigned int devAddr;
	unsigned int numOfNvmeBlock;
	unsigned int page_count;

	if(reqPoolPtr->reqPool[reqSlotTag].reqCode != REQ_CODE_TxDMA)
		return 0;

	numOfNvmeBlock = reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.numOfNvmeBlock;
	if(numOfNvmeBlock != 4 && numOfNvmeBlock != 2)
		return 0;

	page_count = numOfNvmeBlock / (PG_PAGE_SIZE / BYTES_PER_NVME_BLOCK);
	if(page_count == 0 ||
	   page_count > (BYTES_PER_DATA_REGION_OF_SLICE / PG_PAGE_SIZE) ||
	   page_count > max_pages)
		return 0;

	devAddr = GenerateDataBufAddr(reqSlotTag);
	for(unsigned int page_id = 0; page_id < page_count; page_id++)
	{
		unsigned int page_devAddr = devAddr + page_id * PG_PAGE_SIZE;
		uint8_t *base = (uint8_t *)(uintptr_t)page_devAddr;
		uint16_t pd_lower = ((uint16_t)base[13] << 8) | base[12];
		unsigned int tuple_cnt;

		if(pd_lower < 24 || pd_lower > PG_PAGE_SIZE || ((pd_lower - 24) & 0x3))
			return 0;

		tuple_cnt = (pd_lower - 24) >> 2;
		if(tuple_cnt > MAX_TUPLE_PER_PAGE ||
		   tuple_cnt > FILTER_ACCEL_MAX_TUPLES_PER_PAGE)
			return 0;

		page_addr_out[page_id] = page_devAddr;
		pd_lower_out[page_id] = pd_lower;
		tuple_len_out[page_id] = (unsigned short)tuple_cnt;
	}

	*devAddr_out = devAddr;
	*page_count_out = page_count;
	return 1;
}

static void filter_complete_coalesced_reqs(unsigned int req_count)
{
	for(unsigned int i = 0; i < req_count; i++)
		FilterCompleteOrderedReadyReq(filter_coalesce_req_slots[i]);
}

static void filter_finish_coalesced_reqs(unsigned int req_count,
									 unsigned int total_pages,
									 int accel_status)
{
	int result_full = 0;

	if(accel_status == FILTER_ACCEL_OK)
	{
		unsigned int *hit_count_table =
			(unsigned int *)(uintptr_t)FILTER_HIT_DRAM_DATA_BUFFER;

		filter_exec_stats.batch_invocations++;
		filter_exec_stats.batch_pages += total_pages;
		for(unsigned int page_id = 0; page_id < total_pages && !result_full; page_id++)
		{
			unsigned char *page_data = (unsigned char *)(uintptr_t)
				filter_coalesce_page_addr[page_id];
			unsigned int *hit_desc = (unsigned int *)(uintptr_t)(FILTER_HIT_DRAM_DATA_BUFFER +
											 FILTER_ACCEL_BATCH_DESC_OFFSET +
											 page_id * FILTER_ACCEL_BATCH_HIT_STRIDE);
			int fpga_hit_count = filter_collect_fpga_spans(hit_desc,
											   (int)hit_count_table[page_id],
											   filter_fpga_tuple_off,
											   filter_fpga_tuple_len);
#if FILTER_ACCEL_VERIFY_ENABLE
			uint64_t verify_t0 = FilterProfileNow();
			int arm_hit_count = filter_collect_arm_spans(page_data,
											 page_id,
											 filter_coalesce_tuple_len[page_id],
											 csd_qual->natts,
											 csd_qual->filter_cnt,
											 filter_arm_tuple_off,
											 filter_arm_tuple_len);
			unsigned int mismatch = filter_compare_spans(filter_coalesce_pd_lower[page_id],
												 filter_fpga_tuple_off,
												 filter_fpga_tuple_len,
												 fpga_hit_count,
												 filter_arm_tuple_off,
												 filter_arm_tuple_len,
												 arm_hit_count);
			uint64_t verify_cycles = FilterProfileNow() - verify_t0;
			FilterProfileAddVerifyCycles(verify_cycles);
			filter_exec_stats.verify_pages++;
			if(mismatch != 0)
			{
				filter_exec_stats.verify_mismatch_pages++;
				filter_exec_stats.verify_mismatch_tuples += mismatch;
				filter_exec_stats.fpga_discard_pages++;
				filter_exec_stats.fallback_pages++;
				FilterProfileAddArmFallbackCycles(verify_cycles);
				filter_materialize_spans(page_data,
								 filter_coalesce_pd_lower[page_id],
								 filter_arm_tuple_off,
								 filter_arm_tuple_len,
								 arm_hit_count,
								 &result_full);
			}
			else
#endif
			{
				filter_exec_stats.fpga_pages++;
				filter_materialize_fpga_or_arm(page_data,
								 filter_coalesce_pd_lower[page_id],
								 filter_fpga_tuple_off,
								 filter_fpga_tuple_len,
								 fpga_hit_count,
								 page_id,
								 &result_full);
			}
		}
	}
	else
	{
		if(!filter_force_arm_fallback_enabled())
		{
			filter_exec_stats.last_accel_status = accel_status;
			filter_exec_stats.last_hw_status = filter_accel_read_status();
			filter_exec_stats.last_hw_error = filter_accel_read_error();
		}

		for(unsigned int page_id = 0; page_id < total_pages && !result_full; page_id++)
		{
			unsigned char *page_data = (unsigned char *)(uintptr_t)
				filter_coalesce_page_addr[page_id];

			filter_exec_stats.fallback_pages++;
			filter_run_arm_page(page_data,
								filter_coalesce_pd_lower[page_id],
								(int)page_id,
								filter_coalesce_tuple_len[page_id],
								csd_qual->natts,
								csd_qual->filter_cnt,
								&result_full);
		}
	}

	filter_complete_coalesced_reqs(req_count);
}

static unsigned int FilterDrainDeferredReadyReqsCoalesced(unsigned int budget,
													  unsigned int force)
{
	unsigned int base_addr;
	unsigned int first_pages;
	unsigned int total_pages;
	unsigned int req_count;
	unsigned int next_start_index;
	int accel_status = FILTER_ACCEL_ERR_HW;

	(void)force;

#if FILTER_DIRECT_ASYNC_ENABLE
	if(filter_coalesce_async_active)
	{
		accel_status = filter_accel_poll_batch_sg(FILTER_HIT_DRAM_DATA_BUFFER,
												  filter_coalesce_async_page_count,
												  FILTER_ACCEL_BATCH_HIT_STRIDE);
		if(accel_status == FILTER_ACCEL_PENDING)
		{
			filter_coalesce_async_polls++;
			if(filter_coalesce_async_polls <
			   FILTER_DIRECT_ASYNC_TIMEOUT_POLLS_PER_PAGE * filter_coalesce_async_page_count)
				return 0;
			accel_status = FILTER_ACCEL_ERR_TIMEOUT;
		}

		FilterProfileAddFpgaComputeCycles(FilterProfileNow() - filter_coalesce_async_start);
		filter_finish_coalesced_reqs(filter_coalesce_async_req_count,
									 filter_coalesce_async_page_count,
									 accel_status);
		filter_coalesce_async_active = 0;
		filter_coalesce_async_req_count = 0;
		filter_coalesce_async_page_count = 0;
		filter_coalesce_async_polls = 0;
		filter_coalesce_async_start = 0;
#if FILTER_DIRECT_ASYNC_RECHECK_WATERMARK_ENABLE
		if(!force)
			return 0;
#endif
	}
#endif

	if(filter_ready_req_count == 0 || budget == 0)
		return 0;
	if(!FilterReadyReqIsNext())
		return 0;

	filter_coalesce_req_slots[0] = FilterReadyReqAt(0);
	if(!filter_prepare_direct_coalesce_req(filter_coalesce_req_slots[0],
										   &base_addr,
										   &first_pages,
										   filter_coalesce_page_addr,
										   filter_coalesce_pd_lower,
										   filter_coalesce_tuple_len,
										   FILTER_DIRECT_COALESCE_PAGES))
	{
		unsigned int reqSlotTag = FilterPopReadyReq();

		filter_exec_stats.filter_defer_drains++;
		FilterIssueOrderedReadyReq(reqSlotTag);
		return 1;
	}

	total_pages = first_pages;
	req_count = 1;
	next_start_index = FilterReadyReqStartIndex(filter_coalesce_req_slots[0]) +
		FilterReadyReqBlockCount(filter_coalesce_req_slots[0]);
	(void)FilterPopReadyReq();
	filter_exec_stats.filter_defer_drains++;

	while(req_count < budget && filter_ready_req_count > 0 &&
		  total_pages < FILTER_DIRECT_COALESCE_PAGES)
	{
		unsigned int reqSlotTag = FilterReadyReqAt(0);
		unsigned int devAddr;
		unsigned int page_count;
		unsigned int remaining_pages = FILTER_DIRECT_COALESCE_PAGES - total_pages;

		if(FilterReadyReqStartIndex(reqSlotTag) != next_start_index)
			break;
		if(!filter_prepare_direct_coalesce_req(reqSlotTag,
											   &devAddr,
											   &page_count,
											   &filter_coalesce_page_addr[total_pages],
											   &filter_coalesce_pd_lower[total_pages],
											   &filter_coalesce_tuple_len[total_pages],
											   remaining_pages))
			break;

		filter_coalesce_req_slots[req_count] = reqSlotTag;
		(void)FilterPopReadyReq();
		filter_exec_stats.filter_defer_drains++;
		req_count++;
		total_pages += page_count;
		next_start_index += FilterReadyReqBlockCount(reqSlotTag);
	}

	filter_exec_stats.filter_issue_calls += req_count;
	filter_exec_stats.total_pages += total_pages;

	if(!filter_force_arm_fallback_enabled())
	{
		uint64_t t0 = FilterProfileNow();
#if FILTER_DIRECT_ASYNC_ENABLE
		accel_status = filter_accel_start_batch_sg(filter_coalesce_page_addr,
												   FILTER_INFO_DRAM_DATA_BUFFER,
												   FILTER_INFO_DRAM_DATA_BUFFER + 0x1000,
												   FILTER_INFO_DRAM_DATA_BUFFER + 0x2000,
												   FILTER_HIT_DRAM_DATA_BUFFER,
												   0,
												   csd_qual->natts,
												   csd_qual->filter_cnt,
												   filter_coalesce_tuple_len,
												   total_pages,
												   FILTER_ACCEL_BATCH_HIT_STRIDE);
		if(accel_status == FILTER_ACCEL_OK)
		{
			filter_coalesce_async_active = 1;
			filter_coalesce_async_req_count = req_count;
			filter_coalesce_async_page_count = total_pages;
			filter_coalesce_async_polls = 0;
			filter_coalesce_async_start = t0;
			return req_count;
		}
#else
		accel_status = filter_accel_run_batch_sg(filter_coalesce_page_addr,
												 FILTER_INFO_DRAM_DATA_BUFFER,
												 FILTER_INFO_DRAM_DATA_BUFFER + 0x1000,
												 FILTER_INFO_DRAM_DATA_BUFFER + 0x2000,
												 FILTER_HIT_DRAM_DATA_BUFFER,
												 0,
												 csd_qual->natts,
												 csd_qual->filter_cnt,
												 filter_coalesce_tuple_len,
												 total_pages,
												 FILTER_ACCEL_BATCH_HIT_STRIDE);
#endif
		FilterProfileAddFpgaComputeCycles(FilterProfileNow() - t0);
	}
	else
	{
		filter_exec_stats.last_accel_status = -102;
	}

	filter_finish_coalesced_reqs(req_count, total_pages, accel_status);
	return req_count;
}
#endif

#if 0
void IssueFilterLegacy(unsigned int reqSlotTag)
{
	xil_printf("IssueFilter\r\n");
	unsigned int devAddr, dmaIndex, numOfNvmeBlock;

	dmaIndex = reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.startIndex;// 鐞嗚ВstartIndex鐨勫惈涔?
	devAddr = GenerateDataBufAddr(reqSlotTag);
	//print_devAddr_values(devAddr);
	
	numOfNvmeBlock = reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.numOfNvmeBlock;
	if(numOfNvmeBlock != 4 && numOfNvmeBlock != 2)// PG椤典负8KB澶у皬
	{
		xil_printf("numOfNvmeBlock: %d\r\n",reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.numOfNvmeBlock);
		xil_printf("[WARNING] Not supported numOfNvmeBlock [WARNING]\r\n");
		return;
	}
	if(reqPoolPtr->reqPool[reqSlotTag].reqCode == REQ_CODE_TxDMA)
	{
		
		for(int k = 0; k < numOfNvmeBlock; k += 2)
		{
			unsigned int page_devAddr = devAddr + k * BYTES_PER_NVME_BLOCK;
			uint8_t *base = (uint8_t *)(page_devAddr);  // 鎷垮埌褰撳墠椤佃捣濮嬪湴鍧€鐨勬寚閽?
			uint16_t pd_lower = ((uint16_t)base[13] << 8) | base[12];  // pd_lower 鍦≒G涓湁涓や釜瀛楄妭
			int tuple_cnt = max((pd_lower - 24) >> 2,0);
			xil_printf("tuple_cnt: %d\r\n",tuple_cnt);
			int blk_dat_offset = 0;
			int page_id = 0;// 姝ゅ蹇呴』涓?
			int natts = csd_qual->natts;
			int filter_cnt = csd_qual->filter_cnt;
			unsigned char *page_data = (unsigned char*) page_devAddr;
			unsigned short tup_off_array[MAX_TUPLE_PER_PAGE];
			unsigned short tup_len_array[MAX_TUPLE_PER_PAGE];
			unsigned short pos_array[MAX_TUPLE_PER_PAGE*MAX_COL];
			unsigned short len_array[MAX_TUPLE_PER_PAGE*MAX_COL];
			get_tuple_attr_pos(page_data,blk_dat_offset,page_id,natts,
			csd_tupdesc->desc_len_in,csd_tupdesc->desc_align_in,tuple_cnt,
			tup_off_array,tup_len_array,pos_array,len_array);
			
			int hit_off[MAX_TUPLE_PER_PAGE];
			int hit_count_out = 0;
			int use_arm_filter = 1;
#if FILTER_ACCEL_ENABLE
			uint32_t accel_hit_count = 0;
			int accel_status = filter_accel_run_page(page_devAddr,
													 FILTER_INFO_DRAM_DATA_BUFFER,
													 FILTER_INFO_DRAM_DATA_BUFFER + 0x1000,
													 FILTER_INFO_DRAM_DATA_BUFFER + 0x2000,
													 FILTER_HIT_DRAM_DATA_BUFFER,
													 page_id,
													 natts,
													 filter_cnt,
													 tuple_cnt,
													 &accel_hit_count);
			if(accel_status == FILTER_ACCEL_OK)
			{
				unsigned int *hit_index = (unsigned int *)FILTER_HIT_DRAM_DATA_BUFFER;
				hit_count_out = (int)accel_hit_count;
				for(int i = 0; i < hit_count_out && i < MAX_TUPLE_PER_PAGE; i++)
				{
					hit_off[i] = hit_index[i];
				}
				use_arm_filter = 0;
			}
			else
			{
				xil_printf("filter_accel fallback: status=%d hw_status=0x%08x error=0x%08x\r\n",
						   accel_status, filter_accel_read_status(), filter_accel_read_error());
			}
#endif

			if(use_arm_filter)
			{
			filter_tuple(page_data,blk_dat_offset,page_id,natts,filter_cnt,csd_filter,
			tuple_cnt,tup_off_array,tup_len_array,pos_array,len_array,
			hit_off,&hit_count_out);
			}

			xil_printf("hit_count_out: %d\r\n",hit_count_out);
			ItemIdData *item = (ItemIdData*)(page_data + 24);
			for(int i = 0; i < hit_count_out; i ++)
			{
				int index = hit_off[i];
				//xil_printf("hit tuple index: %d\r\n",index);
				ItemIdData *item = (ItemIdData*)(page_data + 24);
				item += index;
				HeapTupleData tuple;
				tuple.t_data =  page_data + item->lp_off;
				tuple.t_len  = item->lp_len;
				if (append_tuple_to_mem_page(&writer, &tuple) < 0)
				{
					xil_printf("Failed to append tuple %d to mem page\r\n", index);
					break;
				}
			}

			int result_page_num = writer.cur_page_index + 1;
			xil_printf("result_page_num : %d \r\n", result_page_num);
			memcpy(FILTER_RESULT_DRAM_DATA_BUFFER,&result_page_num,sizeof(result_page_num));
			//xil_printf("buffer result_page_num : %d \r\n", *(int *)FILTER_RESULT_DRAM_DATA_BUFFER);
		}
	}
	else
	{
		xil_printf("[WARNING] Not supported reqCode [WARNING]\r\n");
	}

}
#endif
void IssueNvmeDmaReq(unsigned int reqSlotTag)
{
	unsigned int devAddr, dmaIndex, numOfNvmeBlock;

	dmaIndex = reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.startIndex;
	devAddr = GenerateDataBufAddr(reqSlotTag);
	//print_devAddr_values(devAddr);

	numOfNvmeBlock = 0;

	if(reqPoolPtr->reqPool[reqSlotTag].reqCode == REQ_CODE_RxDMA)
	{
		while(numOfNvmeBlock < reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.numOfNvmeBlock)
		{
			set_auto_rx_dma(reqPoolPtr->reqPool[reqSlotTag].nvmeCmdSlotTag, dmaIndex, devAddr, NVME_COMMAND_AUTO_COMPLETION_ON);

			numOfNvmeBlock++;
			dmaIndex++;
			devAddr += BYTES_PER_NVME_BLOCK;
		}
		reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.reqTail = g_hostDmaStatus.fifoTail.autoDmaRx;
		reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.overFlowCnt = g_hostDmaAssistStatus.autoDmaRxOverFlowCnt;
	}
	else if(reqPoolPtr->reqPool[reqSlotTag].reqCode == REQ_CODE_TxDMA)
	{
		while(numOfNvmeBlock < reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.numOfNvmeBlock)
		{
			set_auto_tx_dma(reqPoolPtr->reqPool[reqSlotTag].nvmeCmdSlotTag, dmaIndex, devAddr, NVME_COMMAND_AUTO_COMPLETION_ON);

			numOfNvmeBlock++;
			dmaIndex++;
			devAddr += BYTES_PER_NVME_BLOCK;
		}
		reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.reqTail =  g_hostDmaStatus.fifoTail.autoDmaTx;
		reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.overFlowCnt = g_hostDmaAssistStatus.autoDmaTxOverFlowCnt;
	}
	else
		assert(!"[WARNING] Not supported reqCode [WARNING]");
}

void CheckDoneNvmeDmaReq()
{
	unsigned int reqSlotTag, prevReq;
	unsigned int rxDone, txDone;

	reqSlotTag = nvmeDmaReqQ.tailReq;
	rxDone = 0;
	txDone = 0;

	while(reqSlotTag != REQ_SLOT_TAG_NONE)
	{
		prevReq = reqPoolPtr->reqPool[reqSlotTag].prevReq;

		if(reqPoolPtr->reqPool[reqSlotTag].reqCode  == REQ_CODE_RxDMA)
		{
			if(!rxDone)
				rxDone = check_auto_rx_dma_partial_done(reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.reqTail , reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.overFlowCnt);

			if(rxDone)
				SelectiveGetFromNvmeDmaReqQ(reqSlotTag);
		}
		else
		{
			if(!txDone)
				txDone = check_auto_tx_dma_partial_done(reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.reqTail , reqPoolPtr->reqPool[reqSlotTag].nvmeDmaInfo.overFlowCnt);

			if(txDone)
				SelectiveGetFromNvmeDmaReqQ(reqSlotTag);
		}

		reqSlotTag = prevReq;
	}
}



