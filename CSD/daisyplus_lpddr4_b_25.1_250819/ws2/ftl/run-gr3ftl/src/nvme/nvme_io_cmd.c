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

#include "nvme.h"
#include "host_lld.h"
#include "nvme_io_cmd.h"

#include "../ftl_config.h"
#include "../request_transform.h"
#include "filter_common.h"
void handle_nvme_io_filter(unsigned int cmdSlotTag, NVME_IO_COMMAND *nvmeIOCmd)
{
	
	//接收tupdesc filter qual file_extents 
	//xil_printf("handle_nvme_io_filter\r\n");
	unsigned int pCsdBuffer = FILTER_INFO_DRAM_DATA_BUFFER; // CSD 侧内存 buffer 地址,暂时使用这个
	// 初始化接收区
	memset(pCsdBuffer, 0, 4 * 4096);

	//初始化结果集存放区第一页
	init_mem_page(&writer, (unsigned int) FILTER_RESULT_DRAM_DATA_BUFFER);

    unsigned int prp[2];
	unsigned long long fullAddr;
	unsigned int prpLen;
	prpLen = nvmeIOCmd->dword12;      //这里的prpLen代表缓冲区大小（页数），利用dword12这个字段传递
	//xil_printf("prpLen : %d\r\n",prpLen);
	
	prp[0] = nvmeIOCmd->PRP1[0];
    prp[1] = nvmeIOCmd->PRP1[1];
	fullAddr = (unsigned long long)prp[1] << 32 | prp[0];
	
	for(int i = 0; i < 4; i ++)// 设置为4是因为传了 tupdesc filter qual file_extents
	{
		//xil_printf("prp[0] : %u , prp[1]: %u\r\n",prp[0],prp[1]);
		set_direct_rx_dma(pCsdBuffer, prp[1], prp[0], 0x1000);
		fullAddr += 0x1000;
		prp[0] = (unsigned int)(fullAddr & 0xFFFFFFFF);
		prp[1] = (unsigned int)(fullAddr >> 32);
		pCsdBuffer = pCsdBuffer + 0x1000;
		check_direct_rx_dma_done();
	}
	check_direct_rx_dma_done();
	csd_tupdesc = (tupdesc_t *)FILTER_INFO_DRAM_DATA_BUFFER;
	//xil_printf("csd_tupdesc: %d     %d\r\n",csd_tupdesc->desc_align_in[0],csd_tupdesc->desc_align_in[1]);
	csd_filter  = (filter_t *)((char *)FILTER_INFO_DRAM_DATA_BUFFER + 0x1000);
	csd_qual    = (qual_t *)((char *)FILTER_INFO_DRAM_DATA_BUFFER + 0x2000);
	csd_extent_info = (extent_info_t *)((char *)FILTER_INFO_DRAM_DATA_BUFFER + 0x3000);


	//为读取表数据做准备
	//IO_READ_COMMAND_DW12 readInfo12;
	//IO_READ_COMMAND_DW13 readInfo13;
	//IO_READ_COMMAND_DW15 readInfo15;
	unsigned int startLba[2];
	unsigned int nlb;

	//readInfo12.dword = nvmeIOCmd->dword[12];
	//readInfo13.dword = nvmeIOCmd->dword[13];
	//readInfo15.dword = nvmeIOCmd->dword[15];

	startLba[0] = csd_extent_info->start_block + 256;
	startLba[1] = 0;
	nlb = csd_extent_info->block_count - 1;
	//xil_printf("start_block : %d  block_count: %d\r\n",startLba[0],nlb);
	ASSERT(startLba[0] < storageCapacity_L && (startLba[1] < STORAGE_CAPACITY_H || startLba[1] == 0));
	//ASSERT(nlb < MAX_NUM_OF_NLB);
	// ASSERT((nvmeIOCmd->PRP1[0] & 0x3) == 0 && (nvmeIOCmd->PRP2[0] & 0x3) == 0); //error
	// ASSERT(nvmeIOCmd->PRP1[1] < 0x10000 && nvmeIOCmd->PRP2[1] < 0x10000);
	//my_send_string_to_host(nvmeIOCmd);
	ReqTransNvmeToSlice(cmdSlotTag, startLba[0], nlb, IO_NVM_READ);
}
void my_send_string_to_host(NVME_IO_COMMAND *nvmeIOCmd)
{
    xil_printf("my_send_string_to_host\r\n");
    unsigned int pCsdBuffer = ADMIN_CMD_DRAM_DATA_BUFFER; // CSD 侧内存 buffer 地址
    unsigned int prp[2];
    unsigned int prpLen;
	unsigned long long fullAddr;
    // HOST -> CSD

    prpLen = nvmeIOCmd->dword12;      //这里的prpLen代表缓冲区大小（页数），利用dword12这个字段传递
	xil_printf("prpLen : %d\n",prpLen);
    if(prpLen == 0) return;
    // 提取第一页（PRP1）
    // 提取 PRP1 地址 (64 位)
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

    // // 等待 DMA 完成
    // check_direct_rx_dma_done();
    // // xil_printf("Copied string from host : %s\r\n",(char*)ADMIN_CMD_DRAM_DATA_BUFFER);
	// for(int i = 0; i < prpLen; i ++){
	// 	xil_printf("page %d Copied string from host : %s\r\n", i, (char*)(ADMIN_CMD_DRAM_DATA_BUFFER + i * 0x1000));
	// }

	// 先将数据全部写入
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

	ASSERT(startLba[0] < storageCapacity_L && (startLba[1] < STORAGE_CAPACITY_H || startLba[1] == 0));
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
	ASSERT(startLba[0] < storageCapacity_L && (startLba[1] < STORAGE_CAPACITY_H || startLba[1] == 0));
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
	unsigned int pCsdBuffer = ADMIN_CMD_DRAM_DATA_BUFFER; // CSD 侧内存 buffer 地址
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
		case IONVM_FILTER: //0xcc
		{
			//start_time = time(NULL);
			//xil_printf("\r\nIONVM_FILTER Command!!!\r\n");
			//XTime_GetTime(&tStart);
			FILTER_TAG = 1;
			handle_nvme_io_filter(nvmeCmd->cmdSlotTag,nvmeIOCmd);
			// my_handle_io_copy_from_host(nvmeIOCmd,&nvmeCPL);
			// set_auto_nvme_cpl(nvmeCmd->cmdSlotTag, nvmeCPL.specific, nvmeCPL.statusFieldWord);
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

void my_handle_io_copy_from_host(NVME_IO_COMMAND *nvmeIOCmd, NVME_COMPLETION *nvmeCPL)          //传递大缓冲区版本   主机在发送的时候可以进行页对齐，这样就不需要在FPGA端进行余页长度的判断
{

	xil_printf("进入handle_copy_from_host函数\r\n");
    unsigned int pCsdBuffer = ADMIN_CMD_DRAM_DATA_BUFFER; // CSD 侧内存 buffer 地址
    unsigned int prp[2];
    unsigned int prpLen;
	unsigned long long fullAddr;
    // HOST -> CSD

    prpLen = nvmeIOCmd->dword12;      //这里的prpLen代表缓冲区大小（页数），利用dword12这个字段传递
	xil_printf("prpLen : %d\n",prpLen);
    if(prpLen == 0) return;
    // 提取第一页（PRP1）
    // 提取 PRP1 地址 (64 位)
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

    // 填写完成队列条目
    nvmeCPL->dword[0] = 0;
    nvmeCPL->specific = 0x0;
}
void handle_io_copy_from_host(NVME_IO_COMMAND *nvmeIOCmd, NVME_COMPLETION *nvmeCPL)          //传递大缓冲区版本   主机在发送的时候可以进行页对齐，这样就不需要在FPGA端进行余页长度的判断
{

	xil_printf("进入handle_io_copy_from_host函数\n");
    unsigned int pCsdBuffer = 0x00300000; // CSD 侧内存 buffer 地址
    unsigned int prp[2];
    unsigned int prpLen;
	unsigned long long fullAddr;
    // HOST -> CSD

    prpLen = nvmeIOCmd->dword12;      //这里的prpLen代表缓冲区大小（页数），利用dword12这个字段传递
	xil_printf("prpLen : %d\n",prpLen);
	xil_printf("prpLen : %d\n",prpLen);
    if(prpLen == 0) return;
    // 提取第一页（PRP1）
    // 提取 PRP1 地址 (64 位)
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

    // 等待 DMA 完成
    check_direct_rx_dma_done();
    // xil_printf("Copied string from host : %s\r\n",(char*)ADMIN_CMD_DRAM_DATA_BUFFER);
	for(int i = 0; i < prpLen; i ++){
		xil_printf("page %d Copied string from host : %s\n", i, (char*)(ADMIN_CMD_DRAM_DATA_BUFFER + i * 0x1000));
	}

	// 先将数据全部写入
	char *reply = (char *)(pCsdBuffer + prpLen * 0x1000);
	for(int i = 0; i < prpLen; i ++){
		sprintf((char*)(reply + i * 4096), "page %d: hello from CSD", i + 1);
	}

	xil_printf("数据写入完成\n");

	prp[0] = nvmeIOCmd->PRP1[0];
    prp[1] = nvmeIOCmd->PRP1[1];
	fullAddr = (unsigned long long)prp[1] << 32 | prp[0];

	xil_printf("开始发送数据\n");
	for(int i = 0; i < prpLen; i ++){
		set_direct_tx_dma((unsigned int)reply, prp[1], prp[0], 0x1000);
		fullAddr += 0x1000;
		prp[0] = (unsigned int)(fullAddr & 0xFFFFFFFF);
		prp[1] = (unsigned int)(fullAddr >> 32);
		reply += 0x1000;
	}
    check_direct_tx_dma_done();
	xil_printf("数据发送完成\n");

    // 填写完成队列条目
    // nvmeCPL->dword[0] = 0;
    // nvmeCPL->specific = 0x0;
}

