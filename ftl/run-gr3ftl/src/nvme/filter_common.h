#ifndef __FILTER_COMMON_H__
#define __FILTER_COMMON_H__


#include "func_id.h"
#include "page.h"
#include <stdint.h>


#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
//typedef char s8;
typedef short s16;
typedef int	s32;

typedef struct pd_linp{
	unsigned int lp_off:15;
	unsigned int lp_flags:2;
	unsigned int lp_len:15;
}pd_linp_t;

#define MAX_TUPLE_PER_PAGE	128
#define MAX_COL 16

typedef struct tupdesc{
	short desc_len_in[MAX_COL];
	short desc_align_in[MAX_COL];
}tupdesc_t;

//15B
typedef struct pg_clause_op{
	u8 	op_type;		//math|cmp, logic
	u8 op_class;		//double,int, bool
	u16 func_id;
	u8 nargs;
	u8 arg0_tag;
	u16 arg0_index;
	u16 arg0_len;
	u8 arg1_tag;
	u16 arg1_index;
	u16 arg1_len;
}pg_clause_op_t;

//should 64B align
typedef struct filter{
	pg_clause_op_t clause[64];	//1280B
	double  const_float8[64];	//512B
	int 	const_int[128];		//512B
	char	const_char[1024];	//1204B
}filter_t;

typedef struct qual{
	int	natts;
	int 	filter_cnt;
        int 	file_cnt;
	char	filepath[256];
}qual_t;

#define MAX_EXTENTS 64
typedef struct {
    // uint64_t start_byte;    // 起始偏移（字节）
    // uint64_t length_byte;   // 长度（字节）
    uint64_t start_block;   // 起始块（LBA）
    uint64_t block_count;   // 块数
} extent_info_t;
// typedef struct{
//     extent_info_t extents[MAX_EXTENTS];
//     int extent_count;
// } file_extent_info_t;

extern tupdesc_t *csd_tupdesc;
extern filter_t *csd_filter;
extern qual_t *csd_qual;
extern extent_info_t *csd_extent_info;
#define FLOAT8_CMP(cmp)		bool_stack[bool_stack_top] = (arg0 cmp arg1) ? 1 : 0; bool_stack_top++;
#define FLOAT8_MATH(math)	float8_stack[float8_stack_top] = (arg0 math arg1) ; float8_stack_top++;

#define INT_CMP(cmp)		bool_stack[bool_stack_top] = (arg0 cmp arg1) ? 1 : 0; bool_stack_top++;

#define PG_PAGE_SIZE	8192
#define PE_CNT			8
#define PAGE_BUF_WIDTH	64
#define PAGE_BUF_CNT	(PG_PAGE_SIZE*8/PAGE_BUF_WIDTH)
//typedef ap_int<PAGE_BUF_WIDTH> page_buf_t;


void get_tuple_attr_pos(unsigned char buf[8192], int blk_dat_offset, int page_id,
		int natts, short *desc_len_in, short *desc_align_in,
		int page_tuple_cnt,
		unsigned short *tup_off_array,unsigned short *tup_len_array,
		unsigned short *att_pos_array,unsigned short *att_len_array);
void parse_tuple_attr(int index, int natts, short *desc_len_in, short *desc_align_in,
		unsigned char page[8192], int tup_off, int tup_len,
		unsigned short *tup_off_array, unsigned short *tup_len_array,
		unsigned short *att_pos_array, unsigned short *att_len_array);
void filter_tuple(unsigned char buf[8192], int blk_dat_offset, int page_id, int natts,
		int filter_cnt, filter_t *filter,
		int page_tuple_cnt,
		unsigned short *tup_off_array,unsigned short *tup_len_array,
		unsigned short *att_pos_array,unsigned short *att_len_array,
		int hit_off[MAX_TUPLE_PER_PAGE],
		int *hit_count_out);

void print_tuple_content(unsigned char *page, unsigned short tupoff, unsigned short tuplen,
                         unsigned short *att_pos_array, unsigned short *att_len_array, 
						 int i, int natts);

#endif
