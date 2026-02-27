#ifndef NVME_FILTER_H
#define NVME_FILTER_H

#include "common.h"
#define DEV_PATH "nvme0n1"
#define PAGE_NUM 32
#define NSID 15091712
#define OPCODE 0xcc
#define BUFFER_LEN 16 * PAGE_SIZE  //128KB
// 读取 NVMe VSC 数据
int nvme_vsc_send(void* buffer,int fd);
extern int filter_cnt;
extern double total_time;
#endif // NVME_FILTER_H
