//////////////////////////////////////////////////////////////////////////////////
// ftl_config.c for Cosmos+ OpenSSD
// Copyright (c) 2017 Hanyang University ENC Lab.
// Contributed by Yong Ho Song <yhsong@enc.hanyang.ac.kr>
//				  Jaewook Kwak <jwkwak@enc.hanyang.ac.kr>
//				  Sangjin Lee <sjlee@enc.hanyang.ac.kr>
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
// Module Name: Flash Translation Layer Configuration Manager
// File Name: ftl_config.c
//
// Version: v1.0.0
//
// Description:
//   - initialize flash translation layer
//	 - check configuration options
//	 - initialize NAND device
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Revision History:
//
// * v1.0.0
//   - First draft
//////////////////////////////////////////////////////////////////////////////////


#include <assert.h>
#include <stdint.h>
#include "xil_printf.h"
#include "memory_map.h"
#include "t4nsc_ucode.h"
#include "nsc_driver.h"
#include "nvme/page.h"

unsigned int storageCapacity_L;
T4REGS chCtlReg[USER_CHANNELS];
extern unsigned int NSCS[];
extern unsigned int NSC_UCODES[];

static void nfc_install_ucode(unsigned int* bram0);

#if FILTER_ACCEL_NAND_DIAG_ON_BOOT
#define NAND_DIAG_TIMEOUT_POLLS 1000000U
#define NAND_DIAG_RESULT_BYTES  8192U

static char *nand_diag_log_ptr;
static char *nand_diag_log_end;

static void nand_diag_putc(char c)
{
	if(nand_diag_log_ptr < nand_diag_log_end)
		*nand_diag_log_ptr++ = c;
}

static void nand_diag_puts(const char *s)
{
	while(*s)
		nand_diag_putc(*s++);
}

static void nand_diag_hex_nibble(unsigned int value)
{
	value &= 0xfU;
	nand_diag_putc((value < 10U) ? ('0' + value) : ('a' + (value - 10U)));
}

static void nand_diag_hex32(unsigned int value)
{
	int i;
	nand_diag_puts("0x");
	for(i = 7; i >= 0; i--)
		nand_diag_hex_nibble(value >> (i * 4));
}

static void nand_diag_hex_byte(unsigned int value)
{
	nand_diag_hex_nibble(value >> 4);
	nand_diag_hex_nibble(value);
}

static void nand_diag_dec(unsigned int value)
{
	char buf[11];
	int i = 0;

	if(value == 0)
	{
		nand_diag_putc('0');
		return;
	}

	while(value && i < (int)sizeof(buf))
	{
		buf[i++] = (char)('0' + (value % 10U));
		value /= 10U;
	}

	while(i > 0)
		nand_diag_putc(buf[--i]);
}

static void nand_diag_key_hex(const char *key, unsigned int value)
{
	nand_diag_puts(key);
	nand_diag_puts("=");
	nand_diag_hex32(value);
	nand_diag_puts("\r\n");
}

static void nand_diag_key_dec(const char *key, unsigned int value)
{
	nand_diag_puts(key);
	nand_diag_puts("=");
	nand_diag_dec(value);
	nand_diag_puts("\r\n");
}

static int nand_diag_wait_not_busy(T4REGS *regs, const char *stage)
{
	unsigned int polls;

	for(polls = 0; polls < NAND_DIAG_TIMEOUT_POLLS; polls++)
	{
		if(!V2FIsControllerBusy(regs))
			return 0;
	}

	nand_diag_puts("TIMEOUT ");
	nand_diag_puts(stage);
	nand_diag_puts(" queueNotFull wait\r\n");
	nand_diag_key_hex("id_raw", *((volatile unsigned int *)regs->t4regID));
	nand_diag_key_hex("queueNotFull", regs->t4regID->queueNotFull);
	nand_diag_key_hex("queueCount", regs->t4regID->queueCount);
	nand_diag_key_hex("nandReadyBusy", regs->t4regBP->nandReadyBusy);
	return -1;
}

static int nand_diag_wait_word_bit(volatile unsigned int *word, unsigned int mask, const char *stage)
{
	unsigned int polls;

	for(polls = 0; polls < NAND_DIAG_TIMEOUT_POLLS; polls++)
	{
		if((*word & mask) != 0)
			return 0;
	}

	nand_diag_puts("TIMEOUT ");
	nand_diag_puts(stage);
	nand_diag_puts(" word wait value=");
	nand_diag_hex32(*word);
	nand_diag_puts(" mask=");
	nand_diag_hex32(mask);
	nand_diag_puts("\r\n");
	return -1;
}

static void nand_diag_issue_reset(T4REGS *regs, int way)
{
	T4REG_CMD_NAND_RESET resetCmd;

	resetCmd.cmdSelect = T4NSC_CMD_NAND_RESET;
	resetCmd.waySelect = 1U << way;
	V2FFillRegisters(regs, T4REG_CMD_NAND_RESET, resetCmd);
	V2FIssueCommand(regs);
}

static void nand_diag_issue_readybusy(T4REGS *regs, volatile unsigned int *report)
{
	T4REG_CMD_GET_READYBUSY readyBusyCmd;

	*report = 0;
	readyBusyCmd.cmdSelect = T4NSC_CMD_GET_READYBUSY;
	readyBusyCmd.reserved = 0;
	readyBusyCmd.reportAddress = (unsigned int)(uintptr_t)report;
	V2FFillRegisters(regs, T4REG_CMD_GET_READYBUSY, readyBusyCmd);
	V2FIssueCommand(regs);
}

static void nand_diag_issue_set_features(T4REGS *regs, int way, unsigned int address, volatile unsigned int *payload)
{
	T4REG_CMD_SET_FEATURES setFeaturesCmd;

	setFeaturesCmd.cmdSelect = T4NSC_CMD_SET_FEATUREST;
	setFeaturesCmd.waySelect = 1U << way;
	setFeaturesCmd.payloadAddress = (unsigned int)(uintptr_t)payload;
	setFeaturesCmd.setFeaturesAddress = address;
	V2FFillRegisters(regs, T4REG_CMD_SET_FEATURES, setFeaturesCmd);
	V2FIssueCommand(regs);
}

static void nand_diag_issue_read_id(T4REGS *regs, int way, volatile unsigned int *statusReport, volatile unsigned int *completion)
{
	T4REG_CMD_READ_ID readIdCmd;
	int i;

	for(i = 0; i < 8; i++)
		((volatile unsigned char *)statusReport)[i] = 0;
	*completion = 0;

	readIdCmd.cmdSelect = T4NSC_CMD_READ_ID;
	readIdCmd.waySelect = 1U << way;
	readIdCmd.colAddress = 0x0090;
	readIdCmd.length = 3;
	readIdCmd.reportAddress = (unsigned int)(uintptr_t)statusReport;
	readIdCmd.completionReportAddress = (unsigned int)(uintptr_t)completion;
	V2FFillRegisters(regs, T4REG_CMD_READ_ID, readIdCmd);
	V2FIssueCommand(regs);
}

static int nand_diag_wait_way_ready(T4REGS *regs, int way, volatile unsigned int *readyReport, const char *stage)
{
	unsigned int raw;

	if(nand_diag_wait_not_busy(regs, stage) != 0)
		return -1;

	nand_diag_issue_readybusy(regs, readyReport);
	if(nand_diag_wait_word_bit(readyReport, 1U, stage) != 0)
		return -1;

	raw = *readyReport;
	nand_diag_puts(stage);
	nand_diag_puts("_ready_raw=");
	nand_diag_hex32(raw);
	nand_diag_puts(" bits=");
	nand_diag_hex32(raw >> 16);
	nand_diag_puts("\r\n");

	if(((raw >> 16) & (1U << way)) == 0)
	{
		nand_diag_puts("TIMEOUT ");
		nand_diag_puts(stage);
		nand_diag_puts(" way not ready\r\n");
		return -1;
	}

	return 0;
}

static int nand_diag_set_feature_one(T4REGS *regs, int way, unsigned int addr, unsigned int value, volatile unsigned int *payload, volatile unsigned int *readyReport, const char *stage)
{
	*payload = value;
	if(nand_diag_wait_not_busy(regs, stage) != 0)
		return -1;
	nand_diag_issue_set_features(regs, way, addr, payload);
	nand_diag_puts(stage);
	nand_diag_puts("_issued addr=");
	nand_diag_hex32(addr);
	nand_diag_puts(" value=");
	nand_diag_hex32(value);
	nand_diag_puts("\r\n");
	return nand_diag_wait_way_ready(regs, way, readyReport, stage);
}

static int nand_diag_enter_toggle_mode(T4REGS *regs, int way, volatile unsigned int *payload, volatile unsigned int *readyReport)
{
	if(nand_diag_set_feature_one(regs, way, 0x02U, 0x7U, payload, readyReport, "sf_02") != 0)
		return -1;
	if(nand_diag_set_feature_one(regs, way, 0x10U, 0x03U, payload, readyReport, "sf_10") != 0)
		return -1;
	if(nand_diag_set_feature_one(regs, way, 0x91U, 0x100U, payload, readyReport, "sf_91") != 0)
		return -1;

	if(nand_diag_wait_not_busy(regs, "scrambler_off") != 0)
		return -1;
	V2FSetScramblerState(regs, 0);
	nand_diag_puts("scrambler_off_issued\r\n");

	if(nand_diag_set_feature_one(regs, way, 0x01U, 0x24U, payload, readyReport, "sf_01") != 0)
		return -1;

	nand_diag_puts("SET_FEATURE_OK\r\n");
	return 0;
}

static void nand_diag_submit_init_req(unsigned int chNo, unsigned int wayNo, unsigned int reqCode)
{
	unsigned int reqSlotTag;

	reqSlotTag = GetFromFreeReqQ();

	reqPoolPtr->reqPool[reqSlotTag].reqType = REQ_TYPE_NAND;
	reqPoolPtr->reqPool[reqSlotTag].reqCode = reqCode;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandAddr = REQ_OPT_NAND_ADDR_PHY_ORG;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.dataBufFormat = REQ_OPT_DATA_BUF_NONE;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.rowAddrDependencyCheck = REQ_OPT_ROW_ADDR_DEPENDENCY_NONE;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.blockSpace = REQ_OPT_BLOCK_SPACE_TOTAL;
	reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalCh = chNo;
	reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalWay = wayNo;
	reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalBlock = 0;
	reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalPage = 0;
	reqPoolPtr->reqPool[reqSlotTag].prevBlockingReq = REQ_SLOT_TAG_NONE;

	SelectLowLevelReqQ(reqSlotTag);
}

static int nand_diag_scheduler_idle(void)
{
	unsigned int chNo;
	unsigned int wayNo;

	if(nvmeDmaReqQ.headReq != REQ_SLOT_TAG_NONE || nvmeDmaReqQ.reqCnt != 0)
		return 0;
	if(notCompletedNandReqCnt != 0 || blockedReqCnt != 0)
		return 0;

	for(chNo = 0; chNo < USER_CHANNELS; chNo++)
		for(wayNo = 0; wayNo < USER_WAYS; wayNo++)
			if(nandReqQ[chNo][wayNo].headReq != REQ_SLOT_TAG_NONE ||
			   nandReqQ[chNo][wayNo].reqCnt != 0)
				return 0;

	return 1;
}

static void nand_diag_log_scheduler_state(const char *stage)
{
	P_DIE_STATE_TABLE diagDieStateTablePtr = (P_DIE_STATE_TABLE)DIE_STATE_TABLE_ADDR;
	unsigned int chNo;
	unsigned int wayNo;

	nand_diag_puts(stage);
	nand_diag_puts("_STATE_BEGIN\r\n");
	nand_diag_key_hex("nvme_head", nvmeDmaReqQ.headReq);
	nand_diag_key_hex("nvme_tail", nvmeDmaReqQ.tailReq);
	nand_diag_key_dec("nvme_cnt", nvmeDmaReqQ.reqCnt);
	nand_diag_key_dec("notCompletedNandReqCnt", notCompletedNandReqCnt);
	nand_diag_key_dec("blockedReqCnt", blockedReqCnt);
	nand_diag_key_dec("freeReqCnt", freeReqQ.reqCnt);

	for(chNo = 0; chNo < USER_CHANNELS; chNo++)
	{
		nand_diag_puts("SCHED_CH ch=");
		nand_diag_dec(chNo);
		nand_diag_puts(" rb=");
		nand_diag_hex32(chCtlReg[chNo].t4regBP->nandReadyBusy);
		nand_diag_puts(" busy=");
		nand_diag_dec(V2FIsControllerBusy(&chCtlReg[chNo]));
		nand_diag_puts(" idle=");
		nand_diag_hex_nibble(wayPriorityTablePtr->wayPriority[chNo].idleHead);
		nand_diag_putc('/');
		nand_diag_hex_nibble(wayPriorityTablePtr->wayPriority[chNo].idleTail);
		nand_diag_puts(" wr=");
		nand_diag_hex_nibble(wayPriorityTablePtr->wayPriority[chNo].writeHead);
		nand_diag_putc('/');
		nand_diag_hex_nibble(wayPriorityTablePtr->wayPriority[chNo].writeTail);
		nand_diag_puts(" stc=");
		nand_diag_hex_nibble(wayPriorityTablePtr->wayPriority[chNo].statusCheckHead);
		nand_diag_putc('/');
		nand_diag_hex_nibble(wayPriorityTablePtr->wayPriority[chNo].statusCheckTail);
		nand_diag_puts(" str=");
		nand_diag_hex_nibble(wayPriorityTablePtr->wayPriority[chNo].statusReportHead);
		nand_diag_putc('/');
		nand_diag_hex_nibble(wayPriorityTablePtr->wayPriority[chNo].statusReportTail);
		nand_diag_puts("\r\n");

		for(wayNo = 0; wayNo < USER_WAYS; wayNo++)
		{
			unsigned int headReq = nandReqQ[chNo][wayNo].headReq;

			nand_diag_puts("SCHED_WAY ch=");
			nand_diag_dec(chNo);
			nand_diag_puts(" way=");
			nand_diag_dec(wayNo);
			nand_diag_puts(" q=");
			nand_diag_hex32(headReq);
			nand_diag_putc('/');
			nand_diag_hex32(nandReqQ[chNo][wayNo].tailReq);
			nand_diag_puts(" cnt=");
			nand_diag_dec(nandReqQ[chNo][wayNo].reqCnt);
			nand_diag_puts(" die=");
			nand_diag_dec(diagDieStateTablePtr->dieState[chNo][wayNo].dieState);
			nand_diag_puts(" opt=");
			nand_diag_dec(diagDieStateTablePtr->dieState[chNo][wayNo].reqStatusCheckOpt);
			nand_diag_puts(" cf=");
			nand_diag_hex32(completeFlagTablePtr->completeFlag[chNo][wayNo]);
			nand_diag_puts(" sr=");
			nand_diag_hex32(statusReportTablePtr->statusReport[chNo][wayNo]);
			if(headReq != REQ_SLOT_TAG_NONE)
			{
				nand_diag_puts(" type=");
				nand_diag_hex32(reqPoolPtr->reqPool[headReq].reqType);
				nand_diag_puts(" code=");
				nand_diag_hex32(reqPoolPtr->reqPool[headReq].reqCode);
				nand_diag_puts(" qtype=");
				nand_diag_hex32(reqPoolPtr->reqPool[headReq].reqQueueType);
			}
			nand_diag_puts("\r\n");
		}
	}

	nand_diag_puts(stage);
	nand_diag_puts("_STATE_END\r\n");
}

static int nand_diag_wait_scheduler_done(const char *stage)
{
	unsigned int polls;

	for(polls = 0; polls < NAND_DIAG_TIMEOUT_POLLS; polls++)
	{
		CheckDoneNvmeDmaReq();
		SchedulingNandReq();

		if(nand_diag_scheduler_idle())
		{
			nand_diag_puts(stage);
			nand_diag_puts("_OK polls=");
			nand_diag_dec(polls);
			nand_diag_puts("\r\n");
			return 0;
		}
	}

	nand_diag_puts("TIMEOUT ");
	nand_diag_puts(stage);
	nand_diag_puts("\r\n");
	nand_diag_log_scheduler_state(stage);
	return -1;
}

static int nand_diag_wait_scheduler_done_brief(const char *stage, unsigned int chNo, unsigned int wayNo)
{
	unsigned int polls;

	for(polls = 0; polls < NAND_DIAG_TIMEOUT_POLLS; polls++)
	{
		CheckDoneNvmeDmaReq();
		SchedulingNandReq();

		if(nand_diag_scheduler_idle())
		{
			nand_diag_puts(stage);
			nand_diag_puts("_OK polls=");
			nand_diag_dec(polls);
			nand_diag_puts("\r\n");
			return 0;
		}
	}

	nand_diag_puts("TIMEOUT ");
	nand_diag_puts(stage);
	nand_diag_puts(" ch=");
	nand_diag_dec(chNo);
	nand_diag_puts(" way=");
	nand_diag_dec(wayNo);
	nand_diag_puts(" nc=");
	nand_diag_dec(notCompletedNandReqCnt);
	nand_diag_puts(" blk=");
	nand_diag_dec(blockedReqCnt);
	nand_diag_puts(" q=");
	nand_diag_hex32(nandReqQ[chNo][wayNo].headReq);
	nand_diag_putc('/');
	nand_diag_hex32(nandReqQ[chNo][wayNo].tailReq);
	nand_diag_puts(" cnt=");
	nand_diag_dec(nandReqQ[chNo][wayNo].reqCnt);
	nand_diag_puts(" die=");
	nand_diag_dec(((P_DIE_STATE_TABLE)DIE_STATE_TABLE_ADDR)->dieState[chNo][wayNo].dieState);
	nand_diag_puts(" opt=");
	nand_diag_dec(((P_DIE_STATE_TABLE)DIE_STATE_TABLE_ADDR)->dieState[chNo][wayNo].reqStatusCheckOpt);
	nand_diag_puts(" cf=");
	nand_diag_hex32(completeFlagTablePtr->completeFlag[chNo][wayNo]);
	nand_diag_puts(" sr=");
	nand_diag_hex32(statusReportTablePtr->statusReport[chNo][wayNo]);
	nand_diag_puts(" rb=");
	nand_diag_hex32(chCtlReg[chNo].t4regBP->nandReadyBusy);
	nand_diag_puts(" busy=");
	nand_diag_dec(V2FIsControllerBusy(&chCtlReg[chNo]));
	if(nandReqQ[chNo][wayNo].headReq != REQ_SLOT_TAG_NONE)
	{
		unsigned int reqSlotTag = nandReqQ[chNo][wayNo].headReq;
		nand_diag_puts(" code=");
		nand_diag_hex32(reqPoolPtr->reqPool[reqSlotTag].reqCode);
		nand_diag_puts(" qtype=");
		nand_diag_hex32(reqPoolPtr->reqPool[reqSlotTag].reqQueueType);
	}
	nand_diag_puts("\r\n");
	return -1;
}

static void RunNandSchedulerDiagnostic(void)
{
	unsigned int chNo;
	unsigned int wayNo;

	nand_diag_puts("SCHED_DIAG_START\r\n");

	InitChCtlReg();
	InitReqPool();
	InitDependencyTable();
	InitReqScheduler();

	nand_diag_key_dec("sched_free_after_init", freeReqQ.reqCnt);

	for(chNo = 0; chNo < USER_CHANNELS; chNo++)
		for(wayNo = 0; wayNo < USER_WAYS; wayNo++)
		{
			nand_diag_submit_init_req(chNo, wayNo, REQ_CODE_RESET);
			nand_diag_submit_init_req(chNo, wayNo, REQ_CODE_SET_FEATURE);
		}

	nand_diag_key_dec("sched_submitted", notCompletedNandReqCnt);
	nand_diag_wait_scheduler_done("SCHED_RESET_SET_FEATURE");

	nand_diag_puts("SCHED_DIAG_END\r\n");
}

static void nand_diag_init_address_map_ptrs(void)
{
	unsigned int dieNo;

	logicalSliceMapPtr = (P_LOGICAL_SLICE_MAP )LOGICAL_SLICE_MAP_ADDR;
	virtualSliceMapPtr = (P_VIRTUAL_SLICE_MAP)VIRTUAL_SLICE_MAP_ADDR;
	virtualBlockMapPtr = (P_VIRTUAL_BLOCK_MAP)VIRTUAL_BLOCK_MAP_ADDR;
	virtualDieMapPtr = (P_VIRTUAL_DIE_MAP)VIRTUAL_DIE_MAP_ADDR;
	phyBlockMapPtr = (P_PHY_BLOCK_MAP)PHY_BLOCK_MAP_ADDR;
	bbtInfoMapPtr = (P_BAD_BLOCK_TABLE_INFO_MAP)BAD_BLOCK_TABLE_INFO_MAP_ADDR;

	for(dieNo = 0; dieNo < USER_DIES; dieNo++)
	{
		bbtInfoMapPtr->bbtInfo[dieNo].phyBlock = 0;
		bbtInfoMapPtr->bbtInfo[dieNo].grownBadUpdate = BBT_INFO_GROWN_BAD_UPDATE_NONE;
	}
}

static void nand_diag_submit_read_req(unsigned int chNo, unsigned int wayNo, unsigned int blockNo, unsigned int pageNo, unsigned int dataAddr, unsigned int eccOn)
{
	unsigned int reqSlotTag;

	reqSlotTag = GetFromFreeReqQ();

	reqPoolPtr->reqPool[reqSlotTag].reqType = REQ_TYPE_NAND;
	reqPoolPtr->reqPool[reqSlotTag].reqCode = REQ_CODE_READ;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.dataBufFormat = REQ_OPT_DATA_BUF_ADDR;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandAddr = REQ_OPT_NAND_ADDR_PHY_ORG;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandEcc = eccOn ? REQ_OPT_NAND_ECC_ON : REQ_OPT_NAND_ECC_OFF;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandEccWarning = REQ_OPT_NAND_ECC_WARNING_OFF;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.rowAddrDependencyCheck = REQ_OPT_ROW_ADDR_DEPENDENCY_NONE;
	reqPoolPtr->reqPool[reqSlotTag].reqOpt.blockSpace = REQ_OPT_BLOCK_SPACE_TOTAL;
	reqPoolPtr->reqPool[reqSlotTag].dataBufInfo.addr = dataAddr;
	reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalCh = chNo;
	reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalWay = wayNo;
	reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalBlock = blockNo;
	reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalPage = pageNo;
	reqPoolPtr->reqPool[reqSlotTag].prevBlockingReq = REQ_SLOT_TAG_NONE;

	SelectLowLevelReqQ(reqSlotTag);

	nand_diag_puts("READ_SUBMIT tag=");
	nand_diag_dec(reqSlotTag);
	nand_diag_puts(" ch=");
	nand_diag_dec(chNo);
	nand_diag_puts(" way=");
	nand_diag_dec(wayNo);
	nand_diag_puts(" block=");
	nand_diag_dec(blockNo);
	nand_diag_puts(" page=");
	nand_diag_dec(pageNo);
	nand_diag_puts(" ecc=");
	nand_diag_dec(eccOn);
	nand_diag_puts("\r\n");
}

static void nand_diag_log_buf_prefix(unsigned int dataAddr)
{
	unsigned int i;
	volatile unsigned char *p = (volatile unsigned char *)(uintptr_t)dataAddr;

	nand_diag_puts(" data=");
	for(i = 0; i < 16; i++)
	{
		nand_diag_hex_byte(p[i]);
		if(i != 15)
			nand_diag_putc(' ');
	}
}

static void RunNandReadDiagnostic(void)
{
	unsigned int chNo;
	unsigned int wayNo;
	unsigned int dataAddr = RESERVED_DATA_BUFFER_BASE_ADDR;
	unsigned int dieNo;
	unsigned int dataAddrByDie[USER_DIES];

	nand_diag_puts("READ_DIAG_START\r\n");
	nand_diag_init_address_map_ptrs();

	for(chNo = 0; chNo < USER_CHANNELS; chNo++)
		for(wayNo = 0; wayNo < USER_WAYS; wayNo++)
		{
			InitReqPool();
			InitDependencyTable();
			InitReqScheduler();

			memset((void *)(uintptr_t)dataAddr, 0, 64);
			nand_diag_submit_read_req(chNo, wayNo, 0, BAD_BLOCK_MARK_PAGE0, dataAddr, 0);
			if(nand_diag_wait_scheduler_done_brief("READ_B0P0", chNo, wayNo) != 0)
			{
				nand_diag_puts("READ_FAIL ch=");
				nand_diag_dec(chNo);
				nand_diag_puts(" way=");
				nand_diag_dec(wayNo);
				nand_diag_puts("\r\n");
				continue;
			}

			nand_diag_puts("READ_OK ch=");
			nand_diag_dec(chNo);
			nand_diag_puts(" way=");
			nand_diag_dec(wayNo);
			nand_diag_log_buf_prefix(dataAddr);
			nand_diag_puts("\r\n");
		}

	InitReqPool();
	InitDependencyTable();
	InitReqScheduler();
	nand_diag_puts("READ_BATCH_START\r\n");
	for(dieNo = 0; dieNo < USER_DIES; dieNo++)
	{
		chNo = Vdie2PchTranslation(dieNo);
		wayNo = Vdie2PwayTranslation(dieNo);
		dataAddrByDie[dieNo] = RESERVED_DATA_BUFFER_BASE_ADDR + 0x4000 + dieNo * BYTES_PER_NAND_ROW;
		memset((void *)(uintptr_t)dataAddrByDie[dieNo], 0, 64);
		nand_diag_submit_read_req(chNo, wayNo, 0, BAD_BLOCK_MARK_PAGE0, dataAddrByDie[dieNo], 0);
	}
	if(nand_diag_wait_scheduler_done("READ_BATCH") == 0)
	{
		for(dieNo = 0; dieNo < USER_DIES; dieNo++)
		{
			nand_diag_puts("READ_BATCH_OK die=");
			nand_diag_dec(dieNo);
			nand_diag_puts(" ch=");
			nand_diag_dec(Vdie2PchTranslation(dieNo));
			nand_diag_puts(" way=");
			nand_diag_dec(Vdie2PwayTranslation(dieNo));
			nand_diag_log_buf_prefix(dataAddrByDie[dieNo]);
			nand_diag_puts("\r\n");
		}
	}
	else
		nand_diag_puts("READ_BATCH_FAIL\r\n");
	nand_diag_puts("READ_BATCH_END\r\n");

	InitReqPool();
	InitDependencyTable();
	InitReqScheduler();
	nand_diag_puts("READ_BBT_ECC_START\r\n");
	for(dieNo = 0; dieNo < USER_DIES; dieNo++)
	{
		chNo = Vdie2PchTranslation(dieNo);
		wayNo = Vdie2PwayTranslation(dieNo);
		dataAddrByDie[dieNo] = RESERVED_DATA_BUFFER_BASE_ADDR + 0x4000 + dieNo * BYTES_PER_NAND_ROW;
		memset((void *)(uintptr_t)dataAddrByDie[dieNo], 0, 64);
		nand_diag_submit_read_req(chNo, wayNo, 0, START_PAGE_NO_OF_BAD_BLOCK_TABLE_BLOCK, dataAddrByDie[dieNo], 1);
	}
	if(nand_diag_wait_scheduler_done("READ_BBT_ECC") == 0)
	{
		for(dieNo = 0; dieNo < USER_DIES; dieNo++)
		{
			nand_diag_puts("READ_BBT_ECC_OK die=");
			nand_diag_dec(dieNo);
			nand_diag_log_buf_prefix(dataAddrByDie[dieNo]);
			nand_diag_puts("\r\n");
		}
	}
	else
		nand_diag_puts("READ_BBT_ECC_FAIL\r\n");
	nand_diag_puts("READ_BBT_ECC_END\r\n");

	nand_diag_puts("READ_DIAG_END\r\n");
}

static void RunNandBootDiagnostic(void)
{
	memset((void *)FILTER_RESULT_DRAM_DATA_BUFFER, 0, NAND_DIAG_RESULT_BYTES);
	init_mem_page(&writer, (char *)(uintptr_t)FILTER_RESULT_DRAM_DATA_BUFFER);
	nand_diag_log_ptr =
		(char *)(uintptr_t)(FILTER_RESULT_DRAM_DATA_BUFFER + 64U);
	nand_diag_log_end = (char *)(uintptr_t)
		(FILTER_RESULT_DRAM_DATA_BUFFER + NAND_DIAG_RESULT_BYTES - 1U);

	nand_diag_puts("NAND_DIAG_START\r\n");

#if 0
	T4REGS regs;
	volatile unsigned int *statusReport = (volatile unsigned int *)(TEMPORARY_PAY_LOAD_ADDR + 16);
	volatile unsigned int *completion = statusReport + 4;
	volatile unsigned int *readyReport = statusReport + 8;
	volatile unsigned int *featurePayload = statusReport + 12;
	unsigned char idData[8];
	unsigned int i;
	int ch;
	int way;

	for(ch = 0; ch < USER_CHANNELS; ch++)
	{
		nand_diag_puts("CHANNEL_BEGIN ch=");
		nand_diag_dec((unsigned int)ch);
		nand_diag_puts("\r\n");
		nand_diag_key_hex("nsc_base", NSCS[ch]);
		nand_diag_key_hex("ucode_base", NSC_UCODES[ch]);

		nfc_install_ucode((unsigned int *)(uintptr_t)NSC_UCODES[ch]);
		nand_diag_puts("ucode_loaded\r\n");

		V2FInitializeHandle(&regs, (void *)(uintptr_t)NSCS[ch]);
		nand_diag_key_hex("id_raw_initial", *((volatile unsigned int *)regs.t4regID));
		nand_diag_key_hex("queueNotFull_initial", regs.t4regID->queueNotFull);
		nand_diag_key_hex("queueCount_initial", regs.t4regID->queueCount);
		nand_diag_key_hex("nandReadyBusy_initial", regs.t4regBP->nandReadyBusy);

		for(way = 0; way < USER_WAYS; way++)
		{
			nand_diag_puts("WAY_BEGIN ch=");
			nand_diag_dec((unsigned int)ch);
			nand_diag_puts(" way=");
			nand_diag_dec((unsigned int)way);
			nand_diag_puts("\r\n");

			if(nand_diag_wait_not_busy(&regs, "before_reset") != 0)
				continue;
			nand_diag_issue_reset(&regs, way);
			nand_diag_puts("reset_issued\r\n");
			if(nand_diag_wait_not_busy(&regs, "after_reset") != 0)
				continue;
			nand_diag_key_hex("nandReadyBusy_after_reset", regs.t4regBP->nandReadyBusy);

			if(nand_diag_wait_not_busy(&regs, "before_readybusy") != 0)
				continue;
			nand_diag_issue_readybusy(&regs, readyReport);
			nand_diag_puts("readybusy_issued\r\n");
			if(nand_diag_wait_word_bit(readyReport, 1U, "readybusy_report") != 0)
				continue;
			nand_diag_key_hex("readybusy_report_raw", *readyReport);
			nand_diag_key_hex("readybusy_bits", *readyReport >> 16);

			if(nand_diag_enter_toggle_mode(&regs, way, featurePayload, readyReport) != 0)
				continue;

			if(nand_diag_wait_not_busy(&regs, "before_readid") != 0)
				continue;
			nand_diag_issue_read_id(&regs, way, statusReport, completion);
			nand_diag_puts("readid_issued\r\n");
			if(nand_diag_wait_word_bit(completion, 1U, "readid_completion") != 0)
			{
				nand_diag_key_hex("status_word0", statusReport[0]);
				nand_diag_key_hex("status_word1", statusReport[1]);
				continue;
			}

			for(i = 0; i < 6; i++)
				idData[i] = ((volatile unsigned char *)statusReport)[i * 2];

			nand_diag_puts("READID_OK ch=");
			nand_diag_dec((unsigned int)ch);
			nand_diag_puts(" way=");
			nand_diag_dec((unsigned int)way);
			nand_diag_puts(" bytes=");
			for(i = 0; i < 6; i++)
			{
				nand_diag_hex_nibble(idData[i] >> 4);
				nand_diag_hex_nibble(idData[i]);
				if(i != 5)
					nand_diag_putc(' ');
			}
			nand_diag_puts("\r\n");
			nand_diag_key_hex("completion", *completion);
			nand_diag_key_hex("status_word0", statusReport[0]);
			nand_diag_key_hex("status_word1", statusReport[1]);
			nand_diag_puts("WAY_END\r\n");
		}
		nand_diag_puts("CHANNEL_END\r\n");
	}
#endif

	RunNandSchedulerDiagnostic();
	RunNandReadDiagnostic();

	nand_diag_puts("NAND_DIAG_END\r\n");
	*nand_diag_log_ptr = '\0';
	{
		int page_num = 1;
		memcpy((void *)FILTER_RESULT_DRAM_DATA_BUFFER, &page_num, sizeof(page_num));
	}
}
#endif

void InitFTL()
{
#if FTL_BOOT_PROGRESS_ENABLE
	xil_printf("[FTL] InitFTL begin\r\n");
#endif
	CheckConfigRestriction();
#if FTL_BOOT_PROGRESS_ENABLE
	xil_printf("[FTL] CheckConfigRestriction done\r\n");
#endif

#if FILTER_ACCEL_SMOKE_SKIP_NAND_INIT
#if FILTER_ACCEL_NAND_DIAG_ON_BOOT
	RunNandBootDiagnostic();
#endif
	InitReqPool();
	InitDependencyTable();
	InitReqScheduler();
	InitDataBuf();
	InitGcVictimMap();

	storageCapacity_L = 15091712;

	xil_printf("[ smoke mode: NAND init skipped. ]\r\n");
	xil_printf("[ storage capacity %d MB ]\r\n", storageCapacity_L / ((1024*1024) / BYTES_PER_NVME_BLOCK));
	xil_printf("[ ftl configuration complete. ]\r\n");
	return;
#endif

#if FTL_BOOT_PROGRESS_ENABLE
	xil_printf("[FTL] InitChCtlReg begin\r\n");
#endif
	InitChCtlReg();
#if FTL_BOOT_PROGRESS_ENABLE
	xil_printf("[FTL] InitChCtlReg done\r\n");
	xil_printf("[FTL] InitReqPool begin\r\n");
#endif
	InitReqPool();
#if FTL_BOOT_PROGRESS_ENABLE
	xil_printf("[FTL] InitReqPool done\r\n");
	xil_printf("[FTL] InitDependencyTable begin\r\n");
#endif
	InitDependencyTable();
#if FTL_BOOT_PROGRESS_ENABLE
	xil_printf("[FTL] InitDependencyTable done\r\n");
	xil_printf("[FTL] InitReqScheduler begin\r\n");
#endif
	InitReqScheduler();
#if FTL_BOOT_PROGRESS_ENABLE
	xil_printf("[FTL] InitReqScheduler done\r\n");
	xil_printf("[FTL] InitNandArray begin\r\n");
#endif
	InitNandArray();
#if FTL_BOOT_PROGRESS_ENABLE
	xil_printf("[FTL] InitNandArray done\r\n");
	xil_printf("[FTL] InitAddressMap begin\r\n");
#endif
	InitAddressMap();
#if FTL_BOOT_PROGRESS_ENABLE
	xil_printf("[FTL] InitAddressMap done\r\n");
	xil_printf("[FTL] InitDataBuf begin\r\n");
#endif
	InitDataBuf();
#if FTL_BOOT_PROGRESS_ENABLE
	xil_printf("[FTL] InitDataBuf done\r\n");
	xil_printf("[FTL] InitGcVictimMap begin\r\n");
#endif
	InitGcVictimMap();
#if FTL_BOOT_PROGRESS_ENABLE
	xil_printf("[FTL] InitGcVictimMap done\r\n");
#endif

	storageCapacity_L = (MB_PER_SSD - (MB_PER_MIN_FREE_BLOCK_SPACE + mbPerbadBlockSpace + MB_PER_OVER_PROVISION_BLOCK_SPACE)) * ((1024*1024) / BYTES_PER_NVME_BLOCK);

	xil_printf("[ storage capacity %d MB ]\r\n", storageCapacity_L / ((1024*1024) / BYTES_PER_NVME_BLOCK));
	xil_printf("[ ftl configuration complete. ]\r\n");
}

static void nfc_install_ucode(unsigned int* bram0)
{
	int i;
	for (i = 0; i < T4NSCu_Common_CodeWordLength; i++)
	{
		bram0[i] = T4NSCuCode_Common[i];
	}
	for (i = 0; i < T4NSCu_PlainOps_CodeWordLength; i++)
	{
		bram0[T4NSCu_Common_CodeWordLength + i] = T4NSCuCode_PlainOps[i];
	}
}

unsigned int NSCS[] = {
	NSC_0_BASEADDR,
	NSC_1_BASEADDR,
	NSC_2_BASEADDR,
	NSC_3_BASEADDR,
};

unsigned int NSC_UCODES[] = {
	NSC_0_UCODEADDR,
	NSC_1_UCODEADDR,
	NSC_2_UCODEADDR,
	NSC_3_UCODEADDR,
};

static unsigned int dqs_delay[] = {1099, 1099, 1099, 1099};
static unsigned int dq_delay[]  = {00, 00, 00, 00};

#define FTL_BOOT_NAND_INIT_TIMEOUT_MS 10000U
#define FTL_BOOT_NAND_RETRY_LIMIT 3U

void InitChCtlReg()
{
	int i;
	if(USER_CHANNELS < 1)
		assert(!"[WARNING] Configuration Error: Channel [WARNING]");

	for (i = 0; i < USER_CHANNELS; i++)
	{
		nfc_install_ucode((unsigned int *)(uintptr_t)NSC_UCODES[i]);
		V2FInitializeHandle(&chCtlReg[i], (void *)(uintptr_t)NSCS[i]);
	}
}

static void SubmitNandArrayInitReqs(void)
{
	unsigned int chNo, wayNo, reqSlotTag;

	for(chNo=0; chNo<USER_CHANNELS; ++chNo)
		for(wayNo=0; wayNo<USER_WAYS; ++wayNo)
		{
			reqSlotTag = GetFromFreeReqQ();

			reqPoolPtr->reqPool[reqSlotTag].reqType = REQ_TYPE_NAND;
			reqPoolPtr->reqPool[reqSlotTag].reqCode = REQ_CODE_RESET;
			reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandAddr = REQ_OPT_NAND_ADDR_PHY_ORG;
			reqPoolPtr->reqPool[reqSlotTag].reqOpt.dataBufFormat = REQ_OPT_DATA_BUF_NONE;
			reqPoolPtr->reqPool[reqSlotTag].reqOpt.rowAddrDependencyCheck = REQ_OPT_ROW_ADDR_DEPENDENCY_NONE;
			reqPoolPtr->reqPool[reqSlotTag].reqOpt.blockSpace = REQ_OPT_BLOCK_SPACE_TOTAL;
			reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalCh = chNo;
			reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalWay = wayNo;
			reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalBlock = 0;	//dummy
			reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalPage = 0;	//dummy
			reqPoolPtr->reqPool[reqSlotTag].prevBlockingReq = REQ_SLOT_TAG_NONE;

			SelectLowLevelReqQ(reqSlotTag);

			reqSlotTag = GetFromFreeReqQ();

			reqPoolPtr->reqPool[reqSlotTag].reqType = REQ_TYPE_NAND;
			reqPoolPtr->reqPool[reqSlotTag].reqCode = REQ_CODE_SET_FEATURE;
			reqPoolPtr->reqPool[reqSlotTag].reqOpt.nandAddr =  REQ_OPT_NAND_ADDR_PHY_ORG;
			reqPoolPtr->reqPool[reqSlotTag].reqOpt.dataBufFormat = REQ_OPT_DATA_BUF_NONE;
			reqPoolPtr->reqPool[reqSlotTag].reqOpt.rowAddrDependencyCheck = REQ_OPT_ROW_ADDR_DEPENDENCY_NONE;
			reqPoolPtr->reqPool[reqSlotTag].reqOpt.blockSpace = REQ_OPT_BLOCK_SPACE_TOTAL;
			reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalCh = chNo;
			reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalWay = wayNo;
			reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalBlock = 0;	//dummy
			reqPoolPtr->reqPool[reqSlotTag].nandInfo.physicalPage = 0;	//dummy
			reqPoolPtr->reqPool[reqSlotTag].prevBlockingReq = REQ_SLOT_TAG_NONE;

			SelectLowLevelReqQ(reqSlotTag);
		}
}

static void ResetBootNandRequestPipeline(void)
{
	InitChCtlReg();
	InitReqPool();
	InitDependencyTable();
	InitReqScheduler();
}

void InitNandArray()
{
	unsigned int attempt;
	int i; int k;

	for(attempt = 0; attempt < FTL_BOOT_NAND_RETRY_LIMIT; attempt++)
	{
		SubmitNandArrayInitReqs();
		if(SyncAllLowLevelReqDoneTimeout(FTL_BOOT_NAND_INIT_TIMEOUT_MS) == 0)
			break;

		xil_printf("[FTL] NAND init timeout attempt=%u/%u; resetting scheduler\r\n",
			attempt + 1U, FTL_BOOT_NAND_RETRY_LIMIT);
		if(attempt + 1U < FTL_BOOT_NAND_RETRY_LIMIT)
			ResetBootNandRequestPipeline();
	}
	if(attempt == FTL_BOOT_NAND_RETRY_LIMIT)
	{
		xil_printf("[FTL] NAND init failed after bounded retries\r\n");
		assert(!"NAND initialization timeout");
		while(1) { }
	}

#if 0
    for(i = 0; i < 1100; i += 50)
    {
        nfc_set_dqs_delay(0, i);
        for(k = 0; k < 1100; k += 50)
        {
            nfc_set_dq_delay(0, k);

            printf("dqs=%d, dq=%d\r\n", i, k);

            int j;
            unsigned char* idData = (unsigned char*)(TEMPORARY_PAY_LOAD_ADDR + 16);
            V2FReadIdSync(&chCtlReg[0], 0, idData);
            printf("Ch %d Way %d ReadId: ", 0, 0);
            for (j = 0; j < 6;j ++)
                printf("%x ", idData[j]);
            printf("\r\n");
        }
    }
#elif 0
	/*for (i = 0; i < USER_CHANNELS; i++)
	{
		nfc_set_dqs_delay(i, dqs_delay[i]);
		//nfc_set_dq_delay(i, dq_delay[i]);
	}*/

	for (i = 0; i < USER_CHANNELS; i++)
	{
		for (k = 0; k < USER_WAYS; k++)
		{
			int j;
			unsigned char* idData = (unsigned char*)(TEMPORARY_PAY_LOAD_ADDR + 16);
			V2FReadIdSync(&chCtlReg[i], k, idData);
			printf("Ch %d Way %d ReadId: ", i, k);
			for (j = 0; j < 6;j ++)
				printf("%x ", idData[j]);
			printf("\r\n");
		}
	}
#endif
	xil_printf("[ NAND device reset complete. ]\r\n");
}

void RecoverNandArrayForBoot()
{
	ResetBootNandRequestPipeline();
	InitNandArray();
}


void CheckConfigRestriction()
{
	if(USER_CHANNELS > NSC_MAX_CHANNELS)
		assert(!"[WARNING] Configuration Error: Channel [WARNING]");
	if(USER_WAYS > NSC_MAX_WAYS)
		assert(!"[WARNING] Configuration Error: WAY [WARNING]");
	if(USER_BLOCKS_PER_LUN > MAIN_BLOCKS_PER_LUN)
		assert(!"[WARNING] Configuration Error: BLOCK [WARNING]");
	if((BITS_PER_FLASH_CELL != SLC_MODE))
		assert(!"[WARNING] Configuration Error: BIT_PER_FLASH_CELL [WARNING]");

	if(RESERVED_DATA_BUFFER_BASE_ADDR + 0x00200000 > COMPLETE_FLAG_TABLE_ADDR)
		assert(!"[WARNING] Configuration Error: Data buffer size is too large to be allocated to predefined range [WARNING]");
	if(TEMPORARY_PAY_LOAD_ADDR + 0x00001000 > DATA_BUFFER_MAP_ADDR)
		assert(!"[WARNING] Configuration Error: Metadata for NAND request completion process is too large to be allocated to predefined range [WARNING]");
	if(FTL_MANAGEMENT_END_ADDR > DRAM_END_ADDR)
		assert(!"[WARNING] Configuration Error: Metadata of FTL is too large to be allocated to DRAM [WARNING]");
}
