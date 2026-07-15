//////////////////////////////////////////////////////////////////////////////////
// nvme_io_cmd.c for Cosmos+ OpenSSD
// Copyright (c) 2016 Hanyang University ENC Lab.
// Contributed by Yong Ho Song <yhsong@enc.hanyang.ac.kr>
//				  Youngjin Jo <yjjo@enc.hanyang.ac.kr>
//				  Sangjin Lee <sjlee@enc.hanyang.ac.kr>
//				  Jaewook Kwak <jwkwak@enc.hanyang.ac.kr>
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
// Engineer: Sangjin Lee <sjlee@enc.hanyang.ac.kr>
//			 Jaewook Kwak <jwkwak@enc.hanyang.ac.kr>
//
// Project Name: Cosmos+ OpenSSD
// Design Name: Cosmos+ Firmware
// Module Name: NVMe IO Command Handler
// File Name: nvme_io_cmd.c
//
// Version: v1.0.1
//
// Description:
//   - handles NVMe IO command
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Revision History:
//
// * v1.0.1
//   - header file for buffer is changed from "ia_lru_buffer.h" to "lru_buffer.h"
//
// * v1.0.0
//   - First draft
//////////////////////////////////////////////////////////////////////////////////


#include "xil_printf.h"
#include "debug.h"
#include "io_access.h"
#include <stdint.h>
#include <string.h>

#include "nvme.h"
#include "host_lld.h"
#include "nvme_main.h"
#include "nvme_io_cmd.h"
#include "page.h"

#include "../ftl_config.h"
#include "../memory_map.h"
#include "../request_transform.h"
#include "filter_common.h"
#include "filter_accel.h"
#include "filter_compute.h"
#include "filter_join.h"
#include "filter_projection.h"

#define SMOKE_IO_BUFFER_ADDR FILTER_HIT_DRAM_DATA_BUFFER
#define FILTER_EXTENT_LBA_OFFSET 0ULL

#ifndef FILTER_IO_TRACE_ENABLE
#define FILTER_IO_TRACE_ENABLE 0
#endif

#if FILTER_IO_TRACE_ENABLE
#define FILTER_IO_TRACE(...) xil_printf(__VA_ARGS__)
#else
#define FILTER_IO_TRACE(...) do { } while(0)
#endif

static void init_empty_filter_result(void)
{
	int result_page_num = 1;

	FilterExecStatsReset();
	filter_accel_reset_cache();
	filter_projection_disable();
	filter_compute_set_workspace(
		(void *)(uintptr_t)FILTER_COMPUTE_WORK_DRAM_DATA_BUFFER,
		FILTER_COMPUTE_WORK_BYTES);
	filter_compute_disable();
	filter_join_set_workspace(
		(void *)(uintptr_t)FILTER_JOIN_WORK_DRAM_DATA_BUFFER,
		FILTER_JOIN_WORK_BYTES);
	filter_join_disable();
	filter_accel_set_projection(0, 0, 0, 0, 0, 0);
	filter_accel_set_compute(0, 0, 0, 0, 0);
	init_mem_page(&writer, (char *)(uintptr_t)FILTER_RESULT_DRAM_DATA_BUFFER);
	memcpy((void *)(uintptr_t)FILTER_RESULT_DRAM_DATA_BUFFER,
		   &result_page_num, sizeof(result_page_num));
	FilterExecStatsWriteResult();
}

#if FILTER_ACCEL_SMOKE_SKIP_NAND_INIT
static unsigned int get_nlb_count(unsigned int dword12)
{
	IO_READ_COMMAND_DW12 info12;

	info12.dword = dword12;
	return (unsigned int)info12.NLB + 1;
}

static void handle_smoke_io_read(unsigned int cmdSlotTag, NVME_IO_COMMAND *nvmeIOCmd)
{
	unsigned int i;
	unsigned int nlbCount;

	nlbCount = get_nlb_count(nvmeIOCmd->dword12);
	if(nlbCount > 256)
		nlbCount = 256;

	memset((void *)(uintptr_t)SMOKE_IO_BUFFER_ADDR, 0, BYTES_PER_NVME_BLOCK);
	for(i = 0; i < nlbCount; i++)
		set_auto_tx_dma(cmdSlotTag, i, SMOKE_IO_BUFFER_ADDR, (i == (nlbCount - 1)) ? NVME_COMMAND_AUTO_COMPLETION_ON : NVME_COMMAND_AUTO_COMPLETION_OFF);

	check_auto_tx_dma_done();
}

static void handle_smoke_io_write(unsigned int cmdSlotTag, NVME_IO_COMMAND *nvmeIOCmd)
{
	unsigned int i;
	unsigned int nlbCount;

	nlbCount = get_nlb_count(nvmeIOCmd->dword12);
	if(nlbCount > 256)
		nlbCount = 256;

	for(i = 0; i < nlbCount; i++)
		set_auto_rx_dma(cmdSlotTag, i, SMOKE_IO_BUFFER_ADDR, (i == (nlbCount - 1)) ? NVME_COMMAND_AUTO_COMPLETION_ON : NVME_COMMAND_AUTO_COMPLETION_OFF);

	check_auto_rx_dma_done();
}
#endif

void handle_nvme_io_filter(unsigned int cmdSlotTag, NVME_IO_COMMAND *nvmeIOCmd)
{
	
	//鎺ユ敹tupdesc filter qual file_extents 
	FILTER_IO_TRACE("handle_nvme_io_filter\r\n");
	unsigned int pCsdBuffer = FILTER_INFO_DRAM_DATA_BUFFER; // CSD 渚у唴瀛?buffer 鍦板潃,鏆傛椂浣跨敤杩欎釜
	// 鍒濆鍖栨帴鏀跺尯
	memset((void *)(uintptr_t)pCsdBuffer, 0, 4 * 4096);

	//鍒濆鍖栫粨鏋滈泦瀛樻斁鍖虹涓€椤?
	init_empty_filter_result();
    unsigned int prpLen;
    prpLen = nvmeIOCmd->dword12;
    if(prpLen < 4)
    {
        xil_printf("filter metadata page count too small: %d\r\n", prpLen);
		FILTER_TAG = 0;
		set_auto_nvme_cpl(cmdSlotTag, 0x0, 0);
        return;
    }

    uint64_t metadata_dma_t0 = FilterProfileNow();
    for(int i = 0; i < 4; i++)
    {
        set_auto_rx_dma(cmdSlotTag, i, pCsdBuffer, NVME_COMMAND_AUTO_COMPLETION_OFF);
        pCsdBuffer = pCsdBuffer + 0x1000;
    }
    check_auto_rx_dma_done();
    FilterProfileAddHostToCsdDmaCycles(FilterProfileNow() - metadata_dma_t0);
	csd_tupdesc = (tupdesc_t *)(uintptr_t)FILTER_INFO_DRAM_DATA_BUFFER;
	//xil_printf("csd_tupdesc: %d     %d\r\n",csd_tupdesc->desc_align_in[0],csd_tupdesc->desc_align_in[1]);
	csd_filter = (filter_t *)(uintptr_t)(FILTER_INFO_DRAM_DATA_BUFFER + 0x1000U);
	csd_qual = (qual_t *)(uintptr_t)(FILTER_INFO_DRAM_DATA_BUFFER + 0x2000U);
	csd_extent_info =
		(extent_info_t *)(uintptr_t)(FILTER_INFO_DRAM_DATA_BUFFER + 0x3000U);
	FilterSetCountOnlyMode(((unsigned int)csd_qual->file_cnt & FILTER_QUAL_COUNT_ONLY_FLAG) != 0U);

#if FILTER_ACCEL_SMOKE_SKIP_NAND_INIT
	xil_printf("filter smoke mode: metadata received, skip NAND read\r\n");
	set_auto_nvme_cpl(cmdSlotTag, 0x0, 0);
	return;
#endif


	//涓鸿鍙栬〃鏁版嵁鍋氬噯澶?
	//IO_READ_COMMAND_DW12 readInfo12;
	//IO_READ_COMMAND_DW13 readInfo13;
	//IO_READ_COMMAND_DW15 readInfo15;
	unsigned long long startLba64;
	unsigned long long blockCount64;
	unsigned int startLba[2];
	unsigned int nlb;

	//readInfo12.dword = nvmeIOCmd->dword[12];
	//readInfo13.dword = nvmeIOCmd->dword[13];
	//readInfo15.dword = nvmeIOCmd->dword[15];

	startLba64 = csd_extent_info->start_block + FILTER_EXTENT_LBA_OFFSET;
	blockCount64 = csd_extent_info->block_count;

	if(csd_qual->natts <= 0 || csd_qual->natts > MAX_COL ||
	   csd_qual->filter_cnt < 0 || csd_qual->filter_cnt > 64 ||
	   blockCount64 == 0 || blockCount64 > FILTER_MAX_BLOCK_COUNT ||
	   startLba64 >= storageCapacity_L ||
	   (startLba64 + blockCount64) > storageCapacity_L)
	{
		xil_printf("invalid filter metadata: natts=%d filter_cnt=%d start=%u blocks=%u capacity=%u\r\n",
				   csd_qual->natts,
				   csd_qual->filter_cnt,
				   (unsigned int)startLba64,
				   (unsigned int)blockCount64,
				   storageCapacity_L);
		FILTER_TAG = 0;
		set_auto_nvme_cpl(cmdSlotTag, 0x0, 0);
		return;
	}

	{
		int projection_rc = 0;
		int compute_rc = 0;
		int join_rc = 0;
		unsigned int count_only =
			((unsigned int)csd_qual->file_cnt & FILTER_QUAL_COUNT_ONLY_FLAG) != 0U;
		unsigned int enabled_count;
		unsigned int metadata_count = 0U;

#if FILTER_PROJECTION_ENABLE
		const filter_projection_desc_t *projection_desc =
			(const filter_projection_desc_t *)(uintptr_t)
			(FILTER_INFO_DRAM_DATA_BUFFER + 0x2000U + FILTER_PROJECTION_DESC_OFFSET);
		if(projection_desc->magic == FILTER_PROJECTION_DESC_MAGIC)
			metadata_count++;
#endif

#if FILTER_COMPUTE_ENABLE
		const filter_compute_desc_t *compute_desc =
			(const filter_compute_desc_t *)(uintptr_t)
			(FILTER_INFO_DRAM_DATA_BUFFER + 0x2000U + FILTER_COMPUTE_DESC_OFFSET);
		if(compute_desc->magic == FILTER_COMPUTE_DESC_MAGIC)
			metadata_count++;
#endif

#if FILTER_HASH_JOIN_ENABLE
		const filter_join_desc_t *join_desc =
			(const filter_join_desc_t *)(uintptr_t)
			(FILTER_INFO_DRAM_DATA_BUFFER + 0x2000U + FILTER_JOIN_DESC_OFFSET);
		if(join_desc->magic == FILTER_JOIN_DESC_MAGIC)
			metadata_count++;
#endif

		if(metadata_count > 1U)
		{
			xil_printf("conflicting filter result metadata\r\n");
			FILTER_TAG = 0;
			set_auto_nvme_cpl(cmdSlotTag, 0x0, 0);
			return;
		}

#if FILTER_PROJECTION_ENABLE
		projection_rc = filter_projection_configure(projection_desc,
											 (uint32_t)csd_qual->natts,
											 count_only);
#endif
#if FILTER_COMPUTE_ENABLE
		compute_rc = filter_compute_configure(compute_desc,
									  (uint32_t)csd_qual->natts,
									  count_only);
#endif
#if FILTER_HASH_JOIN_ENABLE
		join_rc = filter_join_configure(join_desc,
								 (uint32_t)csd_qual->natts,
								 count_only);
#endif

		enabled_count = (projection_rc > 0 ? 1U : 0U) +
			(compute_rc > 0 ? 1U : 0U) + (join_rc > 0 ? 1U : 0U);
		if(projection_rc < 0 || compute_rc < 0 || join_rc < 0 ||
		   enabled_count > 1U)
		{
			xil_printf("invalid or conflicting filter result metadata\r\n");
			FILTER_TAG = 0;
			set_auto_nvme_cpl(cmdSlotTag, 0x0, 0);
			return;
		}

		if(projection_rc > 0)
		{
			filter_projection_reset_result(
				(void *)(uintptr_t)FILTER_RESULT_DRAM_DATA_BUFFER,
				FILTER_RESULT_MAX_PAGES * PG_PAGE_SIZE);
			filter_accel_set_projection(
				filter_projection_mode() == FILTER_PROJECTION_MODE_FPGA,
				FILTER_PROJECTION_DRAM_DATA_BUFFER,
				FILTER_PROJECTION_STAGING_BYTES_PER_PAGE,
				filter_projection_column_count(),
				filter_projection_max_row_bytes(),
				filter_projection_attr_index());
		}
		else if(compute_rc > 0)
		{
			filter_compute_reset_result(
				(void *)(uintptr_t)FILTER_RESULT_DRAM_DATA_BUFFER,
				FILTER_RESULT_MAX_PAGES * PG_PAGE_SIZE);
			filter_accel_set_compute(
				filter_compute_fpga_enabled(),
				FILTER_COMPUTE_DRAM_DATA_BUFFER,
				FILTER_PROJECTION_STAGING_BYTES_PER_PAGE,
				filter_compute_fpga_config(), 0U);
		}
		else if(join_rc > 0)
		{
			filter_join_reset_result(
				(void *)(uintptr_t)FILTER_RESULT_DRAM_DATA_BUFFER,
				FILTER_RESULT_MAX_PAGES * PG_PAGE_SIZE);
			filter_accel_set_compute(
				filter_join_fpga_enabled(),
				FILTER_COMPUTE_DRAM_DATA_BUFFER,
				FILTER_PROJECTION_STAGING_BYTES_PER_PAGE,
				filter_join_fpga_config(), filter_join_hash_seed());
		}
	}

	startLba[0] = (unsigned int)startLba64;
	startLba[1] = 0;
	nlb = (unsigned int)blockCount64 - 1;
	FILTER_IO_TRACE("start_block : %d  block_count: %d\r\n",startLba[0],nlb);
	ASSERT(startLba[0] < storageCapacity_L);
	//ASSERT(nlb < MAX_NUM_OF_NLB);
	// ASSERT((nvmeIOCmd->PRP1[0] & 0x3) == 0 && (nvmeIOCmd->PRP2[0] & 0x3) == 0); //error
	// ASSERT(nvmeIOCmd->PRP1[1] < 0x10000 && nvmeIOCmd->PRP2[1] < 0x10000);
	//my_send_string_to_host(nvmeIOCmd);
	ReqTransNvmeToSlice(cmdSlotTag, startLba[0], nlb, IO_NVM_READ);
}
void my_send_string_to_host(NVME_IO_COMMAND *nvmeIOCmd)
{
    xil_printf("my_send_string_to_host\r\n");
    unsigned int pCsdBuffer = ADMIN_CMD_DRAM_DATA_BUFFER; // CSD 渚у唴瀛?buffer 鍦板潃
    unsigned int prp[2];
    unsigned int prpLen;
	unsigned long long fullAddr;
    // HOST -> CSD

    prpLen = nvmeIOCmd->dword12;      //杩欓噷鐨刾rpLen浠ｈ〃缂撳啿鍖哄ぇ灏忥紙椤垫暟锛夛紝鍒╃敤dword12杩欎釜瀛楁浼犻€?
	xil_printf("prpLen : %d\n",prpLen);
    if(prpLen == 0) return;
    // 鎻愬彇绗竴椤碉紙PRP1锛?
    // 鎻愬彇 PRP1 鍦板潃 (64 浣?
    prp[0] = nvmeIOCmd->PRP1[0];
    prp[1] = nvmeIOCmd->PRP1[1];

	fullAddr = (unsigned long long)prp[1] << 32 | prp[0];

    // for(int i = 0; i < prpLen; i ++){
	// 	set_direct_rx_dma(pCsdBuffer, prp[1], prp[0], 0x1000);
	// 	fullAddr += 0x1000;
	// 	prp[0] = (unsigned int)(fullAddr & 0xFFFFFFFF);
	// 	prp[1] = (unsigned int)(fullAddr >> 32);
	// 	pCsdBuffer += 0x1000;
	// }

    // // 绛夊緟 DMA 瀹屾垚
    // check_direct_rx_dma_done();
    // // xil_printf("Copied string from host : %s\r\n",(char*)ADMIN_CMD_DRAM_DATA_BUFFER);
	// for(int i = 0; i < prpLen; i ++){
	// 	xil_printf("page %d Copied string from host : %s\r\n", i, (char*)(ADMIN_CMD_DRAM_DATA_BUFFER + i * 0x1000));
	// }

	// 鍏堝皢鏁版嵁鍏ㄩ儴鍐欏叆
	char *reply = (char *)(ADMIN_CMD_DRAM_DATA_BUFFER + prpLen * 0x1000);
	for(int i = 0; i < prpLen; i ++){
		sprintf((char*)(reply + i * 4096), "page %d: hello from CSD", i + 1);
		xil_printf("page %d send string to host : %s\r\n", i, (char*)(reply + i * 0x1000));
	}

	prp[0] = nvmeIOCmd->PRP1[0];
    prp[1] = nvmeIOCmd->PRP1[1];
	fullAddr = (unsigned long long)prp[1] << 32 | prp[0];
	for(int i = 0; i < prpLen; i ++){
		set_direct_tx_dma((unsigned int)reply, prp[1], prp[0], 0x1000);
		fullAddr = (unsigned long long)prp[1] << 32 | prp[0];
		fullAddr += 0x1000;
		prp[0] = (unsigned int)(fullAddr & 0xFFFFFFFF);
		prp[1] = (unsigned int)(fullAddr >> 32);
		reply += 0x1000;
	}
    check_direct_tx_dma_done();
	// NVME_COMPLETION nvmeCPL;
	// nvmeIOCmd = (NVME_IO_COMMAND*)(&nvmeCmd)->cmdDword;
	// nvmeCPL.dword[0] = 0;
	// nvmeCPL.specific = 0x0;
	// set_auto_nvme_cpl(nvmeCmd.cmdSlotTag, nvmeCPL.specific, nvmeCPL.statusFieldWord);
}

void handle_nvme_io_read(unsigned int cmdSlotTag, NVME_IO_COMMAND *nvmeIOCmd)
{
	IO_READ_COMMAND_DW12 readInfo12;
	//IO_READ_COMMAND_DW13 readInfo13;
	//IO_READ_COMMAND_DW15 readInfo15;
	unsigned int startLba[2];
	unsigned int nlb;

	readInfo12.dword = nvmeIOCmd->dword[12];
	//readInfo13.dword = nvmeIOCmd->dword[13];
	//readInfo15.dword = nvmeIOCmd->dword[15];

	startLba[0] = nvmeIOCmd->dword[10];
	startLba[1] = nvmeIOCmd->dword[11];
	nlb = readInfo12.NLB;

#if FILTER_ACCEL_SMOKE_SKIP_NAND_INIT
	(void)startLba;
	(void)nlb;
	handle_smoke_io_read(cmdSlotTag, nvmeIOCmd);
	return;
#endif

	ASSERT(startLba[0] < storageCapacity_L);
	//ASSERT(nlb < MAX_NUM_OF_NLB);
	ASSERT((nvmeIOCmd->PRP1[0] & 0x3) == 0 && (nvmeIOCmd->PRP2[0] & 0x3) == 0); //error
	ASSERT(nvmeIOCmd->PRP1[1] < 0x10000 && nvmeIOCmd->PRP2[1] < 0x10000);

	ReqTransNvmeToSlice(cmdSlotTag, startLba[0], nlb, IO_NVM_READ);
}
void handle_nvme_io_addone(unsigned int cmdSlotTag, NVME_IO_COMMAND *nvmeIOCmd)
{
	handle_nvme_io_read(cmdSlotTag,nvmeIOCmd);
}


void handle_nvme_io_write(unsigned int cmdSlotTag, NVME_IO_COMMAND *nvmeIOCmd)
{
	IO_READ_COMMAND_DW12 writeInfo12;
	//IO_READ_COMMAND_DW13 writeInfo13;
	//IO_READ_COMMAND_DW15 writeInfo15;
	unsigned int startLba[2];
	unsigned int nlb;

	writeInfo12.dword = nvmeIOCmd->dword[12];
	//writeInfo13.dword = nvmeIOCmd->dword[13];
	//writeInfo15.dword = nvmeIOCmd->dword[15];

	//if(writeInfo12.FUA == 1)
	//	xil_printf("write FUA\r\n");

	startLba[0] = nvmeIOCmd->dword[10];
	startLba[1] = nvmeIOCmd->dword[11];
	nlb = writeInfo12.NLB;
	// xil_printf("startLba[0]:%d\r\n",startLba[0]);
	// xil_printf("startLba[1]:%d\r\n",startLba[1]);
	//xil_printf("storageCapacity_L:%d\n",storageCapacity_L);
#if FILTER_ACCEL_SMOKE_SKIP_NAND_INIT
	(void)startLba;
	(void)nlb;
	handle_smoke_io_write(cmdSlotTag, nvmeIOCmd);
	return;
#endif

	ASSERT(startLba[0] < storageCapacity_L);
	//ASSERT(nlb < MAX_NUM_OF_NLB);
	ASSERT((nvmeIOCmd->PRP1[0] & 0xF) == 0 && (nvmeIOCmd->PRP2[0] & 0xF) == 0);
	ASSERT(nvmeIOCmd->PRP1[1] < 0x10000 && nvmeIOCmd->PRP2[1] < 0x10000);

	ReqTransNvmeToSlice(cmdSlotTag, startLba[0], nlb, IO_NVM_WRITE);
}
void handle_io_auto_from_host(unsigned int cmdSlotTag, NVME_IO_COMMAND *nvmeIOCmd)// 0xc3
{
	IO_READ_COMMAND_DW12 writeInfo12;
	unsigned int startLba[2];
	int page_num = nvmeIOCmd->dword[13];
	unsigned int nlb;

	writeInfo12.dword = nvmeIOCmd->dword[12];
	startLba[0] = nvmeIOCmd->dword[10];
	startLba[1] = nvmeIOCmd->dword[11];
	nlb = writeInfo12.NLB;
	unsigned int pCsdBuffer = ADMIN_CMD_DRAM_DATA_BUFFER; // CSD 渚у唴瀛?buffer 鍦板潃
    unsigned int cmd4KBOffset = 0;
	set_auto_rx_dma(cmdSlotTag, cmd4KBOffset, pCsdBuffer, NVME_COMMAND_AUTO_COMPLETION_ON);

	check_auto_rx_dma_done();
	for(int i = 0; i < nlb; i ++)
	{
		xil_printf("%d, Copied string from host : %s\n",i,(char*)(ADMIN_CMD_DRAM_DATA_BUFFER + i*4096));
	}
	

}

void handle_nvme_io_cmd(NVME_COMMAND *nvmeCmd)
{
	unsigned int tag = 0;
	NVME_IO_COMMAND *nvmeIOCmd;
	NVME_COMPLETION nvmeCPL;
	unsigned int opc;

	nvmeIOCmd = (NVME_IO_COMMAND*)nvmeCmd->cmdDword;

	opc = (unsigned int)nvmeIOCmd->OPC;
	//xil_printf("IO Command OPC: 0x%X\r\n", opc);

	switch(opc)
	{
		case IO_NVM_FLUSH:
		{
			PRINT("IO Flush Command\r\n");
			(void)FlushDataBuf(nvmeCmd->cmdSlotTag);
			nvmeCPL.dword[0] = 0;
			nvmeCPL.specific = 0x0;
			set_auto_nvme_cpl(nvmeCmd->cmdSlotTag, nvmeCPL.specific, nvmeCPL.statusFieldWord);
			break;
		}
		case IO_NVM_WRITE:
		{
			
			PRINT("IO Write Command\r\n");
			handle_nvme_io_write(nvmeCmd->cmdSlotTag, nvmeIOCmd);
			break;
		}
		case IO_NVM_READ:
		{
			PRINT("IO Read Command\r\n");
			handle_nvme_io_read(nvmeCmd->cmdSlotTag, nvmeIOCmd);
			break;
		}
		case IO_NVM_WRITE_ZEROS:
		{
			PRINT("IO Write Zeros Command\r\n");
			nvmeCPL.dword[0] = 0;
			nvmeCPL.specific = 0x0;
			set_auto_nvme_cpl(nvmeCmd->cmdSlotTag, nvmeCPL.specific, nvmeCPL.statusFieldWord);
			break;
		}
		case IONVM_FILTER_SEND: //0xcd, host sends filter metadata
		{
			FILTER_IO_TRACE("\r\nIONVM_FILTER Command!!!\r\n");
			FILTER_TAG = 1;
			handle_nvme_io_filter(nvmeCmd->cmdSlotTag,nvmeIOCmd);
			// my_handle_io_copy_from_host(nvmeIOCmd,&nvmeCPL);
			// set_auto_nvme_cpl(nvmeCmd->cmdSlotTag, nvmeCPL.specific, nvmeCPL.statusFieldWord);
			break;
		}
		case IONVM_FILTER_RESULT: //0xce, host reads filter result
		{
			FILTER_IO_TRACE("\r\nIONVM_FILTER_RESULT Command!!!\r\n");
			send_filter_result(nvmeCmd->cmdSlotTag, nvmeIOCmd);
			nvmeCPL.dword[0] = 0;
			nvmeCPL.specific = 0x0;
			set_auto_nvme_cpl(nvmeCmd->cmdSlotTag, nvmeCPL.specific, nvmeCPL.statusFieldWord);
			break;
		}
		case IONVM_FILTER_PROFILE_RESULT:
		{
			FILTER_IO_TRACE("\r\nIONVM_FILTER_PROFILE_RESULT Command!!!\r\n");
			send_filter_profile_result(nvmeCmd->cmdSlotTag, nvmeIOCmd);
			nvmeCPL.dword[0] = 0;
			nvmeCPL.specific = 0x0;
			set_auto_nvme_cpl(nvmeCmd->cmdSlotTag, nvmeCPL.specific, nvmeCPL.statusFieldWord);
			break;
		}
		case IO_NVM_ADDONE: // oxoa
		{
			xil_printf("\r\nIO_NVM_ADDONE Command!!!\r\n");
			ADD_ONE_TAG = 1;
			handle_nvme_io_addone(nvmeCmd->cmdSlotTag, nvmeIOCmd);
			break;
		}
		case 0xc2:
		{
			PRINT("IO Data Transform\r\n");
			//handle_io_copy_from_host(nvmeIOCmd, &nvmeCPL);
			PRINT("IO Data Transform Finish\r\n");
    		break;
		}
		case 0xc3:
		{
			PRINT("IO Data Transform\r\n");
			//handle_io_auto_from_host(nvmeIOCmd, &nvmeCPL);
			PRINT("IO Data Transform Finish\r\n");
    		break;
		}
		case 0xc4:
		{
			xil_printf("0xc4\r\n");
			my_handle_io_copy_from_host(nvmeIOCmd,&nvmeCPL);
			set_auto_nvme_cpl(nvmeCmd->cmdSlotTag, nvmeCPL.specific, nvmeCPL.statusFieldWord);
    		break;
		}

		default:
		{
			xil_printf("Not Support IO Command OPC: 0x%X\r\n", opc);
			ASSERT(0);
			break;
		}
	}

#if (__IO_CMD_DONE_MESSAGE_PRINT)
    xil_printf("OPC = 0x%X\r\n", nvmeIOCmd->OPC);
    xil_printf("PRP1[63:32] = 0x%X, PRP1[31:0] = 0x%X\r\n", nvmeIOCmd->PRP1[1], nvmeIOCmd->PRP1[0]);
    xil_printf("PRP2[63:32] = 0x%X, PRP2[31:0] = 0x%X\r\n", nvmeIOCmd->PRP2[1], nvmeIOCmd->PRP2[0]);
    xil_printf("dword10 = 0x%X\r\n", nvmeIOCmd->dword10);
    xil_printf("dword11 = 0x%X\r\n", nvmeIOCmd->dword11);
    xil_printf("dword12 = 0x%X\r\n", nvmeIOCmd->dword12);
#endif
}

void my_handle_io_copy_from_host(NVME_IO_COMMAND *nvmeIOCmd, NVME_COMPLETION *nvmeCPL)          //浼犻€掑ぇ缂撳啿鍖虹増鏈?  涓绘満鍦ㄥ彂閫佺殑鏃跺€欏彲浠ヨ繘琛岄〉瀵归綈锛岃繖鏍峰氨涓嶉渶瑕佸湪FPGA绔繘琛屼綑椤甸暱搴︾殑鍒ゆ柇
{

	xil_printf("杩涘叆handle_copy_from_host鍑芥暟\r\n");
    unsigned int pCsdBuffer = ADMIN_CMD_DRAM_DATA_BUFFER; // CSD 渚у唴瀛?buffer 鍦板潃
    unsigned int prp[2];
    unsigned int prpLen;
	unsigned long long fullAddr;
    // HOST -> CSD

    prpLen = nvmeIOCmd->dword12;      //杩欓噷鐨刾rpLen浠ｈ〃缂撳啿鍖哄ぇ灏忥紙椤垫暟锛夛紝鍒╃敤dword12杩欎釜瀛楁浼犻€?
	xil_printf("prpLen : %d\n",prpLen);
    if(prpLen == 0) return;
    // 鎻愬彇绗竴椤碉紙PRP1锛?
    // 鎻愬彇 PRP1 鍦板潃 (64 浣?
    prp[0] = nvmeIOCmd->PRP1[0];
    prp[1] = nvmeIOCmd->PRP1[1];

	fullAddr = (unsigned long long)prp[1] << 32 | prp[0];

    for(int i = 0; i < prpLen; i ++){
		set_direct_rx_dma(pCsdBuffer, prp[1], prp[0], 0x1000);
		fullAddr += 0x1000;
		prp[0] = (unsigned int)(fullAddr & 0xFFFFFFFF);
		prp[1] = (unsigned int)(fullAddr >> 32);
		pCsdBuffer += 0x1000;
	}

    // 绛夊緟 DMA 瀹屾垚
    check_direct_rx_dma_done();
    // xil_printf("Copied string from host : %s\r\n",(char*)ADMIN_CMD_DRAM_DATA_BUFFER);
	for(int i = 0; i < prpLen; i ++){
		xil_printf("page %d Copied string from host : %s\r\n", i, (char*)(ADMIN_CMD_DRAM_DATA_BUFFER + i * 0x1000));
	}

	// 鍏堝皢鏁版嵁鍏ㄩ儴鍐欏叆
	char *reply = (char *)(ADMIN_CMD_DRAM_DATA_BUFFER + prpLen * 0x1000);
	for(int i = 0; i < prpLen; i ++){
		sprintf((char*)(reply + i * 4096), "page %d: hello from CSD", i + 1);
	}

	prp[0] = nvmeIOCmd->PRP1[0];
    prp[1] = nvmeIOCmd->PRP1[1];
	fullAddr = (unsigned long long)prp[1] << 32 | prp[0];
	for(int i = 0; i < prpLen; i ++){
		set_direct_tx_dma((unsigned int)reply, prp[1], prp[0], 0x1000);
		fullAddr = (unsigned long long)prp[1] << 32 | prp[0];
		fullAddr += 0x1000;
		prp[0] = (unsigned int)(fullAddr & 0xFFFFFFFF);
		prp[1] = (unsigned int)(fullAddr >> 32);
		reply += 0x1000;
	}
    check_direct_tx_dma_done();

    // 濉啓瀹屾垚闃熷垪鏉＄洰
    nvmeCPL->dword[0] = 0;
    nvmeCPL->specific = 0x0;
}
void handle_io_copy_from_host(NVME_IO_COMMAND *nvmeIOCmd, NVME_COMPLETION *nvmeCPL)          //浼犻€掑ぇ缂撳啿鍖虹増鏈?  涓绘満鍦ㄥ彂閫佺殑鏃跺€欏彲浠ヨ繘琛岄〉瀵归綈锛岃繖鏍峰氨涓嶉渶瑕佸湪FPGA绔繘琛屼綑椤甸暱搴︾殑鍒ゆ柇
{

	xil_printf("杩涘叆handle_io_copy_from_host鍑芥暟\n");
    unsigned int pCsdBuffer = 0x00300000; // CSD 渚у唴瀛?buffer 鍦板潃
    unsigned int prp[2];
    unsigned int prpLen;
	unsigned long long fullAddr;
    // HOST -> CSD

    prpLen = nvmeIOCmd->dword12;      //杩欓噷鐨刾rpLen浠ｈ〃缂撳啿鍖哄ぇ灏忥紙椤垫暟锛夛紝鍒╃敤dword12杩欎釜瀛楁浼犻€?
	xil_printf("prpLen : %d\n",prpLen);
	xil_printf("prpLen : %d\n",prpLen);
    if(prpLen == 0) return;
    // 鎻愬彇绗竴椤碉紙PRP1锛?
    // 鎻愬彇 PRP1 鍦板潃 (64 浣?
    prp[0] = nvmeIOCmd->PRP1[0];
    prp[1] = nvmeIOCmd->PRP1[1];

	fullAddr = (unsigned long long)prp[1] << 32 | prp[0];

    for(int i = 0; i < prpLen; i ++){
		set_direct_rx_dma(pCsdBuffer, prp[1], prp[0], 0x1000);
		fullAddr += 0x1000;
		prp[0] = (unsigned int)(fullAddr & 0xFFFFFFFF);
		prp[1] = (unsigned int)(fullAddr >> 32);
		pCsdBuffer += 0x1000;
	}

    // 绛夊緟 DMA 瀹屾垚
    check_direct_rx_dma_done();
    // xil_printf("Copied string from host : %s\r\n",(char*)ADMIN_CMD_DRAM_DATA_BUFFER);
	for(int i = 0; i < prpLen; i ++){
		xil_printf("page %d Copied string from host : %s\n", i, (char*)(ADMIN_CMD_DRAM_DATA_BUFFER + i * 0x1000));
	}

	// 鍏堝皢鏁版嵁鍏ㄩ儴鍐欏叆
	char *reply = (char *)(pCsdBuffer + prpLen * 0x1000);
	for(int i = 0; i < prpLen; i ++){
		sprintf((char*)(reply + i * 4096), "page %d: hello from CSD", i + 1);
	}

	xil_printf("鏁版嵁鍐欏叆瀹屾垚\n");

	prp[0] = nvmeIOCmd->PRP1[0];
    prp[1] = nvmeIOCmd->PRP1[1];
	fullAddr = (unsigned long long)prp[1] << 32 | prp[0];

	xil_printf("寮€濮嬪彂閫佹暟鎹甛n");
	for(int i = 0; i < prpLen; i ++){
		set_direct_tx_dma((unsigned int)reply, prp[1], prp[0], 0x1000);
		fullAddr += 0x1000;
		prp[0] = (unsigned int)(fullAddr & 0xFFFFFFFF);
		prp[1] = (unsigned int)(fullAddr >> 32);
		reply += 0x1000;
	}
    check_direct_tx_dma_done();
	xil_printf("鏁版嵁鍙戦€佸畬鎴怽n");

    // 濉啓瀹屾垚闃熷垪鏉＄洰
    // nvmeCPL->dword[0] = 0;
    // nvmeCPL->specific = 0x0;
}

