#ifndef PAGE_H
#define PAGE_H
#include "hlog.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#define PAGE_SIZE 8192
#define MAX_PAGES 16
#define PAGE_HEADER_SIZE 24  // sizeof(PageHeaderData)
#define MAXALIGN(x)  (((x) + 7) & ~7)
#define LP_NORMAL		1
#define FLEXIBLE_ARRAY_MEMBER 

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
// typedef struct page{
//     char data[PAGE_SIZE];
// } page;
typedef struct ItemIdData
{
	unsigned	lp_off:15,		/* offset to tuple (from start of page) */
				lp_flags:2,		/* state of item pointer, see below */
				lp_len:15;		/* byte length of tuple */
} ItemIdData;
typedef ItemIdData *ItemId;
typedef struct HeapTupleData
{
	uint32_t		t_len;			/* length of *t_data */
	//ItemPointerData t_self;		/* SelfItemPointer */
	//Oid			t_tabcharleOid;		/* table the tuple came from */
#define FIELDNO_HEAPTUPLEDATA_DATA 3
	char *t_data;		/* -> tuple header and data */
} HeapTupleData;
typedef HeapTupleData* HeapTuple;

typedef struct PageHeaderData
{
	/* XXX LSN is member of *any* block, not only page-organized ones */
	uint64_t pd_lsn;		/* LSN: next byte after last byte of xlog
								 * record for last change to this page */
	uint16_t pd_checksum;	/* checksum */
	uint16_t pd_flags;		/* flag bits, see below */
	uint16_t pd_lower;		/* offset to start of free space */
	uint16_t pd_upper;		/* offset to end of free space */
	uint16_t pd_special;	/* offset to start of special space */
	uint16_t pd_pagesize_version;
	uint32_t pd_prune_xid; /* oldest prunable XID, or zero if none */
	ItemIdData	pd_linp[FLEXIBLE_ARRAY_MEMBER]; /* line pointer array */
} PageHeaderData;


typedef struct MemPageWriter{
    char *base_addr;       // 指向连续内存 (MAX_PAGES * PAGE_SIZE)
    int   cur_page_index;  // 当前页号
    uint16_t free_lower;   // 当前页 line pointer 区的下边界
    uint16_t free_upper;   // 当前页数据区的上边界
    int   tuple_count;     // 当前页已放入的tuple数
} MemPageWriter;

extern MemPageWriter writer;
extern char result_buf[PAGE_SIZE * 16];


/*
 * 初始化第一个页
 */
void init_mem_page(MemPageWriter *w, char *mem_base);

/*
 * 切换到下一个页
 */
int switch_to_next_page(MemPageWriter *w);

/*
 * 把一个 tuple 拷贝到当前页（使用 memcpy）
 */
int append_tuple_to_mem_page(MemPageWriter *w, HeapTuple tuple); // HeapTupleData 只使用到 t_len 和 t_data



#endif // PAGE_H