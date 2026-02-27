//////////////////////////////////////////////////////////////////////////////////
// main.c for Cosmos+ OpenSSD
// Copyright (c) 2016 Hanyang University ENC Lab.
// Contributed by Yong Ho Song <yhsong@enc.hanyang.ac.kr>
//				  Youngjin Jo <yjjo@enc.hanyang.ac.kr>
//				  Sangjin Lee <sjlee@enc.hanyang.ac.kr>
//				  Jaewook Kwak <jwkwak@enc.hanyang.ac.kr>
//				  Kibin Park <kbpark@enc.hanyang.ac.kr>
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
//			 Kibin Park <kbpark@enc.hanyang.ac.kr>
//
// Project Name: Cosmos+ OpenSSD
// Design Name: Cosmos+ Firmware
// Module Name: Main
// File Name: main.c
//
// Version: v1.0.2
//
// Description:
//   - initializes caches, MMU, exception handler
//   - calls nvme_main function
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Revision History:
//
// * v1.0.2
//   - An address region (0x0020_0000 ~ 0x179F_FFFF) is used to uncached & nonbuffered region
//   - An address region (0x1800_0000 ~ 0x3FFF_FFFF) is used to cached & buffered region
//
// * v1.0.1
//   - Paging table setting is modified for QSPI or SD card boot mode
//     * An address region (0x0010_0000 ~ 0x001F_FFFF) is used to place code, data, heap and stack sections
//     * An address region (0x0010_0000 ~ 0x001F_FFFF) is setted a cached&bufferd region
//
// * v1.0.0
//   - First draft
//////////////////////////////////////////////////////////////////////////////////

/*
文件说明：

初始化缓存（Instruction/Data Cache）。

设置 MMU（Memory Management Unit，内存管理单元）。

配置异常处理。

最后调用 nvme_main() 进入 NVMe 控制逻辑

核心任务是 初始化系统资源（缓存、MMU、异常、GIC、NVMe 中断），确保 CPU 与 FPGA 协同工作，再启动 NVMe 控制逻辑。
*/

/*
xil_* 系列：Xilinx 提供的硬件抽象层库，用于：

缓存操作 (xil_cache.h)

异常/中断处理 (xil_exception.h)

MMU 配置 (xil_mmu.h)

中断控制器 (xscugic.h)

打印调试信息 (xil_printf.h)
*/
/*nvme/*：项目内部 NVMe 驱动相关代码，包括调试、主逻辑和主机低层驱动。
*/
#include "xil_cache.h"
#include "xil_exception.h"
#include "xil_mmu.h"
#include "xparameters_ps.h"
#include "xscugic_hw.h"
#include "xscugic.h"
#include "xil_printf.h"
#include "nvme/debug.h"

#include "nvme/nvme.h"
#include "nvme/nvme_main.h"
#include "nvme/host_lld.h"


XScuGic GicInstance;

int main()
{

	
	unsigned int u;

	XScuGic_Config *IntcConfig;
	// 禁用缓存:在配置 MMU/分页表前，需要先关闭缓存，防止旧缓存数据干扰。
	Xil_ICacheDisable();
	Xil_DCacheDisable();

	// 配置 MMU 分页表F
	/*分页策略：

	0x0000_0000 ~ 0x001F_FFFF：缓存可写回（NORM_WB_CACHE）。

	0x0020_0000 ~ 0x179F_FFFF：非缓存区（NORM_NONCACHE）。

	0x1800_0000 ~ 0x3FFF_FFFF：缓存区（NORM_WB_CACHE）。

	其他区域：强制顺序访问（STRONG_ORDERED）。
		
	*/
	// Paging table set
	#define MB (1024*1024)
	for (u = 0; u < 4096; u+=2)
	{
		if (u < 0x2)
			Xil_SetTlbAttributes(u * MB, NORM_WB_CACHE);
		else if (u < 0x180)
			Xil_SetTlbAttributes(u * MB, NORM_NONCACHE);
		else if (u < 0x400)
		
			Xil_SetTlbAttributes(u * MB, NORM_WB_CACHE);
		else if (u < 0x800)
			Xil_SetTlbAttributes(u * MB, NORM_NONCACHE);
		else
			Xil_SetTlbAttributes(u * MB, STRONG_ORDERED);
	}
	// 启用缓存 
	//开启 指令缓存/数据缓存。
	Xil_ICacheEnable();
	Xil_DCacheEnable();
	xil_printf("[!] MMU has been enabled.\r\n");
	//xil_printf("Cjj\n");
	xil_printf("\r\n Hello DaisyPlus OpenSSD !!! \r\n");
	// 初始化异常向量表和默认异常处理函数。
	Xil_ExceptionInit();

	//初始化 GIC 中断控制器
	IntcConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	XScuGic_CfgInitialize(&GicInstance, IntcConfig, IntcConfig->CpuBaseAddress);
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
								(Xil_ExceptionHandler)XScuGic_InterruptHandler,
								&GicInstance);
	// 连接 FPGA 中断
	XScuGic_Connect(&GicInstance, XPS_FPGA0_INT_ID,
					(Xil_ExceptionHandler)dev_irq_handler,
					(void *)0);

	XScuGic_Enable(&GicInstance, XPS_FPGA0_INT_ID);
	//启用 CPU 全局中断
	// Enable interrupts in the Processor.
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
	Xil_ExceptionEnable();
	// 初始化 NVMe 设备中断
	dev_irq_init(); 
	// 调用 NVMe 主逻辑  这里实现 SSD 的 I/O 调度、命令解析和数据传输。
	nvme_main();

	xil_printf("done\r\n");

	return 0;
}
