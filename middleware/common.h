#define _POSIX_C_SOURCE 199309L

#ifndef COMMMON_H
#define COMMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <semaphore.h>
#include <signal.h>
#include "hlog.h"
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <linux/memfd.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include "filter_common.h"
#include <pthread.h>
//#include <linux/un.h>
//#include <thread>
//#include <mutex>
//#include "share_mem.h"
#include <libnvme.h>
#include <sys/time.h>
// #define DEV_PATH "nvme0n1"
//#define PAGE_SIZE 4096
#define SIZE_4KB 4096
#define PAGE_NUM 32
// #define NSID 15091712
#define PAGE_SIZE 8192

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef char s8;
typedef short s16;
typedef int     s32;
#define MAX_COL 16

// typedef struct tupdesc{
//         short desc_len_in[MAX_COL];
//         short desc_align_in[MAX_COL];
// }tupdesc_t;

//15B
// typedef struct pg_clause_op{
//         u8      op_type;                //math|cmp, logic
//         u8 op_class;            //double,int, bool
//         u16 func_id;
//         u8 nargs;
//         u8 arg0_tag;
//         u16 arg0_index;
//         u16 arg0_len;
//         u8 arg1_tag;
//         u16 arg1_index;
//         u16 arg1_len;
// }pg_clause_op_t;

//should 64B align
// typedef struct filter{
//         pg_clause_op_t clause[64];      //1280B
//         double  const_float8[64];       //512B
//         int     const_int[128];         //512B
//         char    const_char[1024];       //1204B
// }filter_t;

// typedef struct qual{
//         int     natts;
//         int     filter_cnt;
//         int     file_cnt;
//         char    filepath[256];
// }qual_t;

// typedef struct {
//     uint64_t start_block;   // 起始块（LBA）
//     uint64_t block_count;   // 块数
// } extent_info_t;
typedef struct page_t{
    char data[PAGE_SIZE];
} page_t;

#endif
