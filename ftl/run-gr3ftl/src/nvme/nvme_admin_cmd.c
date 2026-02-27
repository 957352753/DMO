//////////////////////////////////////////////////////////////////////////////////
// nvme_admin_cmd.c for Cosmos+ OpenSSD
// Copyright (c) 2016 Hanyang University ENC Lab.
// Contributed by Yong Ho Song <yhsong@enc.hanyang.ac.kr>
//				  Youngjin Jo <yjjo@enc.hanyang.ac.kr>
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
// Engineer: Sangjin Lee <sjlee@enc.hanyang.ac.kr>
//
// Project Name: Cosmos+ OpenSSD
// Design Name: Cosmos+ Firmware
// Module Name: NVMe Admin Command Handler
// File Name: nvme_admin_cmd.c
//
// Version: v1.0.0
//
// Description:
//   - handles NVMe admin command
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Revision History:
//
// * v1.0.0
//   - First draft
//////////////////////////////////////////////////////////////////////////////////


#include "xil_printf.h"
#include "debug.h"
#include "string.h"
#include "io_access.h"
#include "../memory_map.h"

#include "nvme.h"
#include "host_lld.h"
#include "nvme_identify.h"
#include "nvme_admin_cmd.h"
#include <stdint.h>
extern NVME_CONTEXT g_nvmeTask;

unsigned int set_num_of_queue(unsigned int dword11)
{
	ADMIN_SET_FEATURES_NUMBER_OF_QUEUES_DW11 requested;
	ADMIN_SET_FEATURES_NUMBER_OF_QUEUES_COMPLETE allocated;

	requested.dword = dword11;
	xil_printf("Number of IO Submission Queues Requested (NSQR, zero-based): 0x%04X\r\n", requested.NSQR);
	xil_printf("Number of IO Completion Queues Requested (NCQR, zero-based): 0x%04X\r\n", requested.NCQR);

	//IO submission queue allocating
	if(requested.NSQR >= MAX_NUM_OF_IO_SQ)
		g_nvmeTask.numOfIOSubmissionQueuesAllocated = MAX_NUM_OF_IO_SQ;
	else
		g_nvmeTask.numOfIOSubmissionQueuesAllocated = requested.NSQR + 1;//zero-based -> non zero-based

	allocated.NSQA = g_nvmeTask.numOfIOSubmissionQueuesAllocated - 1;//non zero-based -> zero-based


	//IO completion queue allocating
	if(requested.NCQR >= MAX_NUM_OF_IO_CQ)
		g_nvmeTask.numOfIOCompletionQueuesAllocated = MAX_NUM_OF_IO_CQ;
	else
		g_nvmeTask.numOfIOCompletionQueuesAllocated = requested.NCQR + 1;//zero-based -> non zero-based

	allocated.NCQA = g_nvmeTask.numOfIOCompletionQueuesAllocated - 1;//non zero-based -> zero-based

	xil_printf("Number of IO Submission Queues Allocated (NSQA, zero-based): 0x%04X\r\n", allocated.NSQA);
	xil_printf("Number of IO Completion Queues Allocated (NCQA, zero-based): 0x%04X\r\n", allocated.NCQA);

	return allocated.dword;
}

unsigned int get_num_of_queue(unsigned int dword10)
{
	ADMIN_GET_FEATURES_NUMBER_OF_QUEUES_COMPLETE allocated;

	allocated.NCQA = g_nvmeTask.numOfIOCompletionQueuesAllocated - 1;//non zero-based -> zero-based
	allocated.NSQA = g_nvmeTask.numOfIOSubmissionQueuesAllocated - 1;//non zero-based -> zero-based

	return allocated.dword;
}

void handle_set_features(NVME_ADMIN_COMMAND *nvmeAdminCmd, NVME_COMPLETION *nvmeCPL)
{
	ADMIN_SET_FEATURES_DW10 features;

	features.dword = nvmeAdminCmd->dword10;

	switch(features.FID)
	{
		case NUMBER_OF_QUEUES:
		{
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = set_num_of_queue(nvmeAdminCmd->dword11);
			break;
		}
		case INTERRUPT_COALESCING:
		{
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = 0x0;
			break;
		}
		case ARBITRATION:
		{
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = 0x0;
			break;
		}
		case ASYNCHRONOUS_EVENT_CONFIGURATION:
		{
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = 0x0;
			break;
		}
		case VOLATILE_WRITE_CACHE:
		{
			xil_printf("Set VWC: 0x%X\r\n", nvmeAdminCmd->dword11);
			g_nvmeTask.cacheEn = (nvmeAdminCmd->dword11 & 0x1);
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = 0x0;
			break;
		}
		case POWER_MANAGEMENT:
		{
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = 0x0;
			break;
		}
		case TIMESTAMP:
		{
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = 0x0;
			break;
		}
		case 0x80:
		{
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = 0x0;
			break;
		}
		default:
		{
			xil_printf("Not Support FID (Set): 0x%X\r\n", features.FID);
			ASSERT(0);
			break;
		}
	}
	if(__ADMIN_CMD_DONE_MESSAGE_PRINT)
    	xil_printf("Set Feature FID:0x%X\r\n", features.FID);
}

void handle_get_features(NVME_ADMIN_COMMAND *nvmeAdminCmd, NVME_COMPLETION *nvmeCPL)
{
	ADMIN_GET_FEATURES_DW10 features;
	NVME_COMPLETION cpl;

	features.dword = nvmeAdminCmd->dword10;

	switch(features.FID)
	{
		case NUMBER_OF_QUEUES:
		{
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = get_num_of_queue(nvmeAdminCmd->dword10);
			break;
		}
		case LBA_RANGE_TYPE:
		{
			//ASSERT(nvmeAdminCmd->NSID == 1);

			cpl.dword[0] = 0x0;
			cpl.statusField.SC = SC_INVALID_FIELD_IN_COMMAND;
			nvmeCPL->dword[0] = cpl.dword[0];
			nvmeCPL->specific = 0x0;
			break;
		}
		case TEMPERATURE_THRESHOLD:
		{
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = nvmeAdminCmd->dword11;
			break;
		}
		case VOLATILE_WRITE_CACHE:
		{
			
			xil_printf("Get VWC: 0x%X\r\n", g_nvmeTask.cacheEn);
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = g_nvmeTask.cacheEn;
			break;
		}
		case POWER_MANAGEMENT:
		{
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = 0x0;
			break;
		}
		case POWER_STATE_TRANSITION:
		{
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = 0x0;
			break;
		}
		case 0xD0:
		{
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = 0x0;
			break;
		}
		case 0x80:
		{
			nvmeCPL->dword[0] = 0x0;
			nvmeCPL->specific = 0x0;
			break;
		}
		default:
		{
			xil_printf("Not Support FID (Get): 0x%X\r\n", features.FID);
			ASSERT(0);
			break;
		}
	}
	if(__ADMIN_CMD_DONE_MESSAGE_PRINT)
    	xil_printf("Get Feature FID: 0x%X\r\n", features.FID);
}

void handle_create_io_sq(NVME_ADMIN_COMMAND *nvmeAdminCmd, NVME_COMPLETION *nvmeCPL)
{
	ADMIN_CREATE_IO_SQ_DW10 sqInfo10;
	ADMIN_CREATE_IO_SQ_DW11 sqInfo11;
	NVME_IO_SQ_STATUS *ioSqStatus;
	unsigned int ioSqIdx;

	sqInfo10.dword = nvmeAdminCmd->dword10;
	sqInfo11.dword = nvmeAdminCmd->dword11;

	xil_printf("Create IO SQ, DW11: 0x%08X, DW10: 0x%08X\r\n", sqInfo11.dword, sqInfo10.dword);

	ASSERT((nvmeAdminCmd->PRP1[0] & 0x3) == 0 && nvmeAdminCmd->PRP1[1] < 0x10000);
	ASSERT(0 < sqInfo10.QID && sqInfo10.QID <= 8 && sqInfo10.QSIZE < 0x100 && 0 < sqInfo11.CQID && sqInfo11.CQID <= 8);

	ioSqIdx = sqInfo10.QID - 1;
	ioSqStatus = g_nvmeTask.ioSqInfo + ioSqIdx;

	ioSqStatus->valid = 1;
	ioSqStatus->qSzie = sqInfo10.QSIZE;
	ioSqStatus->cqVector = sqInfo11.CQID;
	ioSqStatus->pcieBaseAddrL = nvmeAdminCmd->PRP1[0];
	ioSqStatus->pcieBaseAddrH = nvmeAdminCmd->PRP1[1];

	set_io_sq(ioSqIdx, ioSqStatus->valid, ioSqStatus->cqVector, ioSqStatus->qSzie, ioSqStatus->pcieBaseAddrL, ioSqStatus->pcieBaseAddrH);

	nvmeCPL->dword[0] = 0;
	nvmeCPL->specific = 0x0;

}

void handle_delete_io_sq(NVME_ADMIN_COMMAND *nvmeAdminCmd, NVME_COMPLETION *nvmeCPL)
{
	ADMIN_DELETE_IO_SQ_DW10 sqInfo10;
	NVME_IO_SQ_STATUS *ioSqStatus;
	unsigned int ioSqIdx;

	sqInfo10.dword = nvmeAdminCmd->dword10;

	xil_printf("Delete IO SQ, DW10: 0x%08X\r\n", sqInfo10.dword);

	ioSqIdx = (unsigned int)sqInfo10.QID - 1;
	ioSqStatus = g_nvmeTask.ioSqInfo + ioSqIdx;

	ioSqStatus->valid = 0;
	ioSqStatus->cqVector = 0;
	ioSqStatus->qSzie = 0;
	ioSqStatus->pcieBaseAddrL = 0;
	ioSqStatus->pcieBaseAddrH = 0;

	set_io_sq(ioSqIdx, 0, 0, 0, 0, 0);

	nvmeCPL->dword[0] = 0;
	nvmeCPL->specific = 0x0;
}


void handle_create_io_cq(NVME_ADMIN_COMMAND *nvmeAdminCmd, NVME_COMPLETION *nvmeCPL)
{
	ADMIN_CREATE_IO_CQ_DW10 cqInfo10;
	ADMIN_CREATE_IO_CQ_DW11 cqInfo11;
	NVME_IO_CQ_STATUS *ioCqStatus;
	unsigned int ioCqIdx;

	cqInfo10.dword = nvmeAdminCmd->dword10;
	cqInfo11.dword = nvmeAdminCmd->dword11;

	xil_printf("Create IO CQ, DW11: 0x%08X, DW10: 0x%08X\r\n", cqInfo11.dword, cqInfo10.dword);

	ASSERT(((nvmeAdminCmd->PRP1[0] & 0x3) == 0) && (nvmeAdminCmd->PRP1[1] < 0x10000));
	ASSERT(cqInfo11.IV < 8 && cqInfo10.QSIZE < 0x100 && 0 < cqInfo10.QID && cqInfo10.QID <= 8);

	ioCqIdx = cqInfo10.QID - 1;
	ioCqStatus = g_nvmeTask.ioCqInfo + ioCqIdx;

	ioCqStatus->valid = 1;
	ioCqStatus->qSzie = cqInfo10.QSIZE;
	ioCqStatus->irqEn = cqInfo11.IEN;
	ioCqStatus->irqVector = cqInfo11.IV;
	ioCqStatus->pcieBaseAddrL = nvmeAdminCmd->PRP1[0];
	ioCqStatus->pcieBaseAddrH = nvmeAdminCmd->PRP1[1];

	set_io_cq(ioCqIdx, ioCqStatus->valid, ioCqStatus->irqEn, ioCqStatus->irqVector, ioCqStatus->qSzie, ioCqStatus->pcieBaseAddrL, ioCqStatus->pcieBaseAddrH);

	nvmeCPL->dword[0] = 0;
	nvmeCPL->specific = 0x0;
}

void handle_delete_io_cq(NVME_ADMIN_COMMAND *nvmeAdminCmd, NVME_COMPLETION *nvmeCPL)
{
	ADMIN_DELETE_IO_CQ_DW10 cqInfo10;
	NVME_IO_CQ_STATUS *ioCqStatus;
	unsigned int ioCqIdx;

	cqInfo10.dword = nvmeAdminCmd->dword10;

	xil_printf("Delete IO CQ, DW10: 0x%08X\r\n", cqInfo10.dword);

	ioCqIdx = (unsigned int)cqInfo10.QID - 1;
	ioCqStatus = g_nvmeTask.ioCqInfo + ioCqIdx;

	ioCqStatus->valid = 0;
	ioCqStatus->irqVector = 0;
	ioCqStatus->qSzie = 0;
	ioCqStatus->pcieBaseAddrL = 0;
	ioCqStatus->pcieBaseAddrH = 0;
	
	set_io_cq(ioCqIdx, 0, 0, 0, 0, 0, 0);

	nvmeCPL->dword[0] = 0;
	nvmeCPL->specific = 0x0;
}
//  NVMe SSD 控制器固件里处理“Identify”  Admin 命令的完整流程。
void handle_identify(NVME_ADMIN_COMMAND *nvmeAdminCmd, NVME_COMPLETION *nvmeCPL)
{
	
	ADMIN_IDENTIFY_COMMAND_DW10 identifyInfo;
	unsigned int pIdentifyData = ADMIN_CMD_DRAM_DATA_BUFFER;
	unsigned int prp[2];
	unsigned int prpLen;
	/*dword10 的最低字节是 CNS（Controller/Namespace Structure），决定本次要取的是“控制器身份信息”还是“命名空间身份信息”。*/
	identifyInfo.dword = nvmeAdminCmd->dword10;

	if(identifyInfo.CNS == 1)//CI: Controller Identify
	{
		if((nvmeAdminCmd->PRP1[0] & 0x3) != 0 || (nvmeAdminCmd->PRP2[0] & 0x3) != 0)
			xil_printf("CI: PRP1 = 0x%08X_%08X, PRP2 = %08X_%08X\r\n", nvmeAdminCmd->PRP1[1], nvmeAdminCmd->PRP1[0], nvmeAdminCmd->PRP2[1], nvmeAdminCmd->PRP2[0]);

		ASSERT((nvmeAdminCmd->PRP1[0] & 0x3) == 0 && (nvmeAdminCmd->PRP2[0] & 0x3) == 0);
		controller_identification(pIdentifyData);
	}
	else if(identifyInfo.CNS == 0)//NI: Namespace Identify
	{
		if((nvmeAdminCmd->PRP1[0] & 0x3) != 0 || (nvmeAdminCmd->PRP2[0] & 0x3) != 0)
			xil_printf("NI: 0xPRP1 = %08X_%08X, PRP2 = %08X_%08X\r\n", nvmeAdminCmd->PRP1[1], nvmeAdminCmd->PRP1[0], nvmeAdminCmd->PRP2[1], nvmeAdminCmd->PRP2[0]);

		//ASSERT(nvmeAdminCmd->NSID == 1);
		ASSERT((nvmeAdminCmd->PRP1[0] & 0x3) == 0 && (nvmeAdminCmd->PRP2[0] & 0x3) == 0);
		namespace_identification(pIdentifyData);
	}
	else
		ASSERT(0);
	
	prp[0] = nvmeAdminCmd->PRP1[0];
	prp[1] = nvmeAdminCmd->PRP1[1];

	prpLen = 0x1000 - (prp[0] & 0xFFF);
//	xil_printf("prpLen = %X, prp[1] = %X, prp[0] = %X\r\n",prpLen, prp[1], prp[0]);
	set_direct_tx_dma(pIdentifyData, prp[1], prp[0], prpLen);
	if(prpLen != 0x1000)
	{
		pIdentifyData = pIdentifyData + prpLen;
		prpLen = 0x1000 - prpLen;
		prp[0] = nvmeAdminCmd->PRP2[0];
		prp[1] = nvmeAdminCmd->PRP2[1];

//		ASSERT((prp[1] & 0xFFF) == 0);
//		xil_printf("prpLen = %X, prp[1] = %X, prp[0] = %X\r\n",prpLen, prp[1], prp[0]);
		set_direct_tx_dma(pIdentifyData, prp[1], prp[0], prpLen);
	}

	check_direct_tx_dma_done();
	nvmeCPL->dword[0] = 0;
	nvmeCPL->specific = 0x0;
}

void handle_get_log_page(NVME_ADMIN_COMMAND *nvmeAdminCmd, NVME_COMPLETION *nvmeCPL)
{
	/*ADMIN_GET_LOG_PAGE_DW10 getLogPageInfo;

	unsigned int prp1[2];
	unsigned int prp2[2];
	unsigned int prpLen;

	getLogPageInfo.dword = nvmeAdminCmd->dword10;

	prp1[0] = nvmeAdminCmd->PRP1[0];
	prp1[1] = nvmeAdminCmd->PRP1[1];
	prpLen = 0x1000 - (prp1[0] & 0xFFF);

	prp2[0] = nvmeAdminCmd->PRP2[0];
	prp2[1] = nvmeAdminCmd->PRP2[1];

	xil_printf("ADMIN GET LOG PAGE\n");

	//LID
	//Mandatory//1-Error information, 2-SMART/Health information, 3-Firmware Slot information
	//Optional//4-ChangedNamespaceList, 5-Command Effects Log
	xil_printf("LID: 0x%X, NUMD: 0x%X \r\n", getLogPageInfo.LID, getLogPageInfo.NUMD);

	xil_printf("PRP1[63:32] = 0x%X, PRP1[31:0] = 0x%X", prp1[1], prp1[0]);
	xil_printf("PRP2[63:32] = 0x%X, PRP2[31:0] = 0x%X", prp2[1], prp2[0]);*/

	nvmeCPL->dword[0] = 0;
    nvmeCPL->statusField.SCT = 1;
	nvmeCPL->specific = 0x9;//invalid log page
}

uint32_t host2dram(uint32_t pcie_addr)
{
    return pcie_addr + 0x20000000;   // OpenSSD 固定偏移
}

void handle_copy_from_host1(NVME_ADMIN_COMMAND *nvmeAdminCmd, NVME_COMPLETION *nvmeCPL)
{
    unsigned int pCsdBuffer = ADMIN_CMD_DRAM_DATA_BUFFER; // CSD 侧内存 buffer 地址
    unsigned int prp[2];
    unsigned int prpLen;
	// HOST -> CSD
    // 提取 PRP1 地址 (64 位)
    prp[0] = nvmeAdminCmd->PRP1[0];
    prp[1] = nvmeAdminCmd->PRP1[1];
	// 打印 PRP1 的高低 32 位
	xil_printf("PRP1 low 32-bit : 0x%08X\r\n", prp[0]);
	xil_printf("PRP1 high 32-bit: 0x%08X\r\n", prp[1]);

	// 如果要拼成 64 位完整物理地址
	unsigned long long prp1_addr = ((unsigned long long)prp[1] << 32) | prp[0];
	xil_printf("PRP1 full 64-bit addr: 0x%016llX\r\n", prp1_addr);
    // 计算剩余页长度
    prpLen = 0x1000 - (prp[0] & 0xFFF);

    // 先拷 PRP1 指向的那一段
    set_direct_rx_dma(pCsdBuffer, prp[1], prp[0], prpLen);

    if(prpLen != 0x1000)
    {
        // 更新 CSD buffer 指针
        pCsdBuffer = pCsdBuffer + prpLen;

        // 剩余部分由 PRP2 提供
        prpLen = 0x1000 - prpLen;
        prp[0] = nvmeAdminCmd->PRP2[0];
        prp[1] = nvmeAdminCmd->PRP2[1];

        set_direct_rx_dma(pCsdBuffer, prp[1], prp[0], prpLen);
    }

    // 等待 DMA 完成
    check_direct_rx_dma_done();
	xil_printf("Copied string from host : %s\r\n",(char*)ADMIN_CMD_DRAM_DATA_BUFFER);


	// CSD -> HOST

	char *reply = (char *)(ADMIN_CMD_DRAM_DATA_BUFFER + 0x1000);
    strcpy(reply, "hello from CSD");
	xil_printf("CSD send : %s\r\n",(char*)reply);
    // 用 PRP1 作为返回目的地址
    prp[0] = nvmeAdminCmd->PRP1[0];
    prp[1] = nvmeAdminCmd->PRP1[1];
	prpLen = 0x1000 - (prp[0] & 0xFFF);
    //prpLen = strlen(reply) + 1;  // 包含 '\0'
    set_direct_tx_dma((unsigned int)reply, prp[1], prp[0], prpLen);

	if(prpLen != 0x1000)
	{
		reply = reply + prpLen;
		prpLen = 0x1000 - prpLen;
		prp[0] = nvmeAdminCmd->PRP2[0];
		prp[1] = nvmeAdminCmd->PRP2[1];

//		ASSERT((prp[1] & 0xFFF) == 0);
//		xil_printf("prpLen = %X, prp[1] = %X, prp[0] = %X\r\n",prpLen, prp[1], prp[0]);
		set_direct_tx_dma(reply, prp[1], prp[0], prpLen);
	}
    check_direct_tx_dma_done();

    // 填写完成队列条目
    nvmeCPL->dword[0] = 0;
    nvmeCPL->specific = 0x0;
}



/**
lk add
*/

void verify_dma_data(unsigned int pcieAddrL, unsigned int pcieAddrH, const char *expected) {
    unsigned long long host_addr = ((unsigned long long)pcieAddrH << 32) | pcieAddrL;
    char *host_data = (char *)host_addr;
    if (strncmp(host_data, expected, 15) == 0) { // 共 15 字符
        xil_printf("Data verified at 0x%016llx: %s\r\n", host_addr, expected);
    } else {
        xil_printf("Verification failed: expected %s, got %s\r\n", expected, host_data);
    }
}

void handle_nvme_admin_cmd(NVME_COMMAND *nvmeCmd)
{

	NVME_ADMIN_COMMAND *nvmeAdminCmd;
	NVME_COMPLETION nvmeCPL;
	unsigned int opc;
	unsigned int needCpl;
	unsigned int needSlotRelease;

	nvmeAdminCmd = (NVME_ADMIN_COMMAND*)nvmeCmd->cmdDword;
	opc = (unsigned int)nvmeAdminCmd->OPC;
	xil_printf("nvmeAdminCmd->OPC : 0x%X\r\n",opc);
	
	needCpl = 1;
	needSlotRelease = 0;
	switch(opc)
	{
		case ADMIN_SET_FEATURES:
		{
			handle_set_features(nvmeAdminCmd, &nvmeCPL);
			break;
		}
		case ADMIN_CREATE_IO_CQ:
		{
			handle_create_io_cq(nvmeAdminCmd, &nvmeCPL);
			break;
		}
		case ADMIN_CREATE_IO_SQ:
		{
			handle_create_io_sq(nvmeAdminCmd, &nvmeCPL);
			break;
		}
		case ADMIN_IDENTIFY:
		{
			PRINT("ADMIN_IDENTIFY\r\n");
			handle_identify(nvmeAdminCmd, &nvmeCPL);
			break;
		}
		case ADMIN_GET_FEATURES:
		{
			handle_get_features(nvmeAdminCmd, &nvmeCPL);
			break;
		}
		case ADMIN_DELETE_IO_CQ:
		{
			handle_delete_io_cq(nvmeAdminCmd, &nvmeCPL);
			break;
		}
		case ADMIN_DELETE_IO_SQ:
		{
			handle_delete_io_sq(nvmeAdminCmd, &nvmeCPL);
			break;
		}
		case ADMIN_ASYNCHRONOUS_EVENT_REQUEST:
		{
			needCpl = 0;
			needSlotRelease = 1;
			nvmeCPL.dword[0] = 0;
			nvmeCPL.specific = 0x0;
			break;
		}
		case ADMIN_GET_LOG_PAGE:
		{
			handle_get_log_page(nvmeAdminCmd, &nvmeCPL);
			break;
		}
		case ADMIN_SECURITY_RECEIVE:
		{
			needCpl = 0;
			needSlotRelease = 0;
			nvmeCPL.dword[0] = 0;
			nvmeCPL.specific = 0x0;
			break;
		}
		case ADMIN_DOORBELL_BUFFER_CONFIG:
		{
			needCpl = 0;
			needSlotRelease = 0;
			nvmeCPL.dword[0] = 0;
			nvmeCPL.specific = 0x0;
			break;
		}
		case ADMIN_ABORT:// 0x8
		{
			nvmeCPL.dword[0] = 0;
			nvmeCPL.specific = 0x0;
			break;
		}

		case 0xc0:
		{
			xil_printf("OPC 0xC0: calc 1+2=3, return 5 as demo\r\n");
    		
    		nvmeCPL.dword[0] = 0;          // 清 status
    		nvmeCPL.specific   = 0x0;        // <-- 这就是 CQE.DW0，主机会收到
    		break;
		}

		case 0xc1:
		{
			// uint32_t tx_len  = nvmeAdminCmd->dword[10];
    		// uint32_t host_tx = (nvmeAdminCmd->PRP1[1] << 32) | nvmeAdminCmd->PRP1[0];
    		// uint32_t host_rx = (nvmeAdminCmd->PRP2[1] << 32) | nvmeAdminCmd->PRP2[0];

    		// char buf[64];
    		// /* 读 */
    		// set_direct_rx_dma(host2dram(host_tx), 0, (uint32_t)buf, tx_len);
    		// check_direct_rx_dma_done();
    		// buf[tx_len] = 0;
    		// xil_printf("CSD recv: '%s'\r\n", buf);

    		// /* 写 */
    		// const char *reply = "success";
    		// set_direct_tx_dma((uint32_t)reply, 0, host2dram(host_rx), strlen(reply)+1);
    		// check_direct_tx_dma_done();

    		// nvmeCPL.specific = strlen(reply)+1;
    		break;
			
		}
		case 0xc2:
		{
			xil_printf("OPC 0xc2 Begin\n");
    		handle_copy_from_host(nvmeAdminCmd, &nvmeCPL);
    		break;
		}
		case 0xc3:
		{
			// xil_printf("OPC 0xc3 Begin\n");
    		// handle_auto_copy_from_host(nvmeAdminCmd, &nvmeCPL,nvmeCmd);
    		break;
		}
		case 0xc4:
		{
			// xil_printf("OPC 0xc4 Begin\n");
			// handle_copy_from_host1(nvmeAdminCmd, &nvmeCPL);
			break;
		}
		default:
		{
			xil_printf("Not Support Admin Command OPC: 0x%X\r\n", opc);
			nvmeCPL.statusFieldWord = 0;
			nvmeCPL.specific = 0x0;
			nvmeCPL.statusField.DNR = 1;
			nvmeCPL.statusField.SCT = 0;
			nvmeCPL.statusField.SC = 1;
			break;
		}
	}

	if(needCpl == 1)
		set_auto_nvme_cpl(nvmeCmd->cmdSlotTag, nvmeCPL.specific, nvmeCPL.statusFieldWord);
	else if(needSlotRelease == 1)
		set_nvme_slot_release(nvmeCmd->cmdSlotTag);
	else

	set_nvme_cpl(nvmeCmd->qID, nvmeAdminCmd->CID, nvmeCPL.specific, nvmeCPL.statusFieldWord);

	if(__ADMIN_CMD_DONE_MESSAGE_PRINT)
		xil_printf("Admin Command Done, OPC: 0x%02X\r\n", opc);
}



void handle_copy_from_host(NVME_ADMIN_COMMAND *nvmeAdminCmd, NVME_COMPLETION *nvmeCPL)          //传递大缓冲区版本   主机在发送的时候可以进行页对齐，这样就不需要在FPGA端进行余页长度的判断
{

	xil_printf("进入handle_copy_from_host函数\r\n");
    unsigned int pCsdBuffer = ADMIN_CMD_DRAM_DATA_BUFFER; // CSD 侧内存 buffer 地址
    unsigned int prp[2];
    unsigned int prpLen;
	unsigned long long fullAddr;
    // HOST -> CSD

    prpLen = nvmeAdminCmd->dword12;      //这里的prpLen代表缓冲区大小（页数），利用dword12这个字段传递
	xil_printf("prpLen : %d\n",prpLen);
    if(prpLen == 0) return;
    // 提取第一页（PRP1）
    // 提取 PRP1 地址 (64 位)
    prp[0] = nvmeAdminCmd->PRP1[0];
    prp[1] = nvmeAdminCmd->PRP1[1];

	fullAddr = (unsigned long long)prp[1] << 32 | prp[0];

    for(int i = 0; i < prpLen; i ++){
		set_direct_rx_dma(pCsdBuffer, prp[1], prp[0], 0x1000);
		fullAddr += 0x1000;
		prp[0] = (unsigned int)(fullAddr & 0xFFFFFFFF);
		prp[1] = (unsigned int)(fullAddr >> 32);
		pCsdBuffer += 0x1000;
	}

    // 等待 DMA 完成
    check_direct_rx_dma_done();
    // xil_printf("Copied string from host : %s\r\n",(char*)ADMIN_CMD_DRAM_DATA_BUFFER);
	for(int i = 0; i < prpLen; i ++){
		xil_printf("page %d Copied string from host : %s\r\n", i, (char*)(ADMIN_CMD_DRAM_DATA_BUFFER + i * 0x1000));
	}

	// 先将数据全部写入
	char *reply = (char *)(ADMIN_CMD_DRAM_DATA_BUFFER + prpLen * 0x1000);
	for(int i = 0; i < prpLen; i ++){
		sprintf((char*)(reply + i * 4096), "page %d: hello from CSD", i + 1);
	}

	prp[0] = nvmeAdminCmd->PRP1[0];
    prp[1] = nvmeAdminCmd->PRP1[1];
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

    // 填写完成队列条目
    nvmeCPL->dword[0] = 0;
    nvmeCPL->specific = 0x0;
}

void handle_auto_copy_from_host(NVME_ADMIN_COMMAND *nvmeAdminCmd, NVME_COMPLETION *nvmeCPL,NVME_COMMAND *nvmeCmd)
{
    unsigned int pCsdBuffer = ADMIN_CMD_DRAM_DATA_BUFFER; // CSD 侧内存 buffer 地址
    unsigned int cmdSlotTag;
    unsigned int cmd4KBOffset = 0;
    unsigned int autoCompletion = 1; // 让硬件自动发completion

    // 1️⃣ 获取命令槽位号（一般由提交队列头或者门铃寄存器计算）
    cmdSlotTag = nvmeCmd->cmdSlotTag;  // CID 通常对应 slotTag，如果你的 FW 有其他获取方式就用自己的

    // 2️⃣ HOST -> CSD：使用 AUTO RX DMA 拷贝
    set_auto_rx_dma(cmdSlotTag, cmd4KBOffset, pCsdBuffer, autoCompletion);

    // 等待 DMA 完成
    check_auto_rx_dma_done();
    xil_printf("Copied string from host : %s\r\n",(char*)ADMIN_CMD_DRAM_DATA_BUFFER);

    // 3️⃣ CSD -> HOST：准备返回字符串
    char *reply = (char *)(ADMIN_CMD_DRAM_DATA_BUFFER + 0x1000);
    strcpy(reply, "1234 from CSD");
    xil_printf("CSD send : %s\r\n",(char*)reply);

    // 使用 AUTO TX DMA 发送回复
    cmd4KBOffset = 0;  // 从命令数据区第0个4KB开始
    set_auto_tx_dma(cmdSlotTag, cmd4KBOffset, (unsigned int)reply, autoCompletion);

    // 等待 DMA 完成
    check_auto_tx_dma_done();

    // 填写完成队列条目
    nvmeCPL->dword[0] = 0;
    nvmeCPL->specific = 0x0;
}