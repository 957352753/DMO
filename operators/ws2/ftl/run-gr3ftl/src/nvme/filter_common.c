#include "filter_common.h"
#include "func_id.h"
#include <stdbool.h>
#include <stdint.h>
#include "xil_printf.h"
#include "string.h"

tupdesc_t *csd_tupdesc ;
filter_t *csd_filter ;
qual_t *csd_qual;
extent_info_t *csd_extent_info;

#ifndef FILTER_COMMON_TRACE_ENABLE
#define FILTER_COMMON_TRACE_ENABLE 0
#endif

#if FILTER_COMMON_TRACE_ENABLE
#define FILTER_COMMON_TRACE(...) xil_printf(__VA_ARGS__)
#else
#define FILTER_COMMON_TRACE(...) do { } while(0)
#endif

static int filter_memcmp_payload(const unsigned char *lhs, unsigned short lhs_len,
								 const unsigned char *rhs, unsigned short rhs_len)
{
	unsigned short min_len = (lhs_len < rhs_len) ? lhs_len : rhs_len;
	int cmp = 0;

	if (min_len > 0)
		cmp = memcmp(lhs, rhs, min_len);
	if (cmp != 0)
		return cmp;
	if (lhs_len == rhs_len)
		return 0;
	return (lhs_len < rhs_len) ? -1 : 1;
}

void parse_tuple_attr(int index, int natts, short *desc_len_in, short *desc_align_in,
		unsigned char page[8192], int tup_off, int tup_len,
		unsigned short *tup_off_array, unsigned short *tup_len_array,
		unsigned short *att_pos_array, unsigned short *att_len_array)
{
	short desc_len, desc_align;
	unsigned short pos_index;

	unsigned short pos_index_p;
	unsigned short len_dat1byte, len_dat4byte;
	unsigned short att_off, att_len;
	//xil_printf("index : %d, tup_off:%d, tup_len: %d \r\n",index,tup_off,tup_len);
	pos_index = page[tup_off + 22];
	for(int i=0; i<natts; i++){
		desc_len = desc_len_in[i];
		desc_align = desc_align_in[i];

		pos_index_p = (pos_index + desc_align);
		desc_align = ~desc_align;
		pos_index_p = (pos_index_p) & desc_align;
		if(desc_len > 0){
			att_off = pos_index_p;
			att_len = (unsigned short)desc_len;
			pos_index =  pos_index_p + desc_len;
			*(att_pos_array + index*MAX_COL + i) = att_off;
			*(att_len_array + index*MAX_COL + i) = att_len;
		}else{
			unsigned char meta_len = 1;
			len_dat1byte = page[tup_off + pos_index];
			if(0 == len_dat1byte){
				pos_index = pos_index_p;
			}
			memcpy(&len_dat4byte, page + tup_off + pos_index , 2);
			if(len_dat4byte & 0x1){
				len_dat4byte = (len_dat4byte >> 1)&0x7f;
			}else{
				len_dat4byte = (len_dat4byte >> 2)&0xffff;
				meta_len = 4;
			}
			att_off = pos_index;
			att_len = len_dat4byte;
			pos_index = pos_index + len_dat4byte;
			*(att_pos_array + index*MAX_COL + i) = att_off + meta_len;
			*(att_len_array + index*MAX_COL + i) = att_len - meta_len;
		}

	}

	tup_off_array[index] = tup_off;
	tup_len_array[index] = tup_len;
}
void get_tuple_attr_pos(unsigned char buf[8192], int blk_dat_offset, int page_id,
		int natts, short *desc_len_in, short *desc_align_in,
		int page_tuple_cnt,
		unsigned short *tup_off_array,unsigned short *tup_len_array,
		unsigned short *att_pos_array,unsigned short *att_len_array)//blk_dat_offset page_id 可以删除
{
	unsigned char * page = (unsigned char *)buf;
	pd_linp_t *item;
	PageHeaderData *hdr = (PageHeaderData *)page;
	unsigned short pd_lower = hdr->pd_lower;

	(void)blk_dat_offset;
	(void)page_id;

	item = (pd_linp_t*)(page + 24);
	for(int i=0; i<page_tuple_cnt; i++){
		//xil_printf("index : %d,tuple_flag: %d tup_off : %d tup_len: %d\r\n",i,item[i].lp_flags,item[i].lp_off,item[i].lp_len);
		tup_off_array[i] = 0;
		tup_len_array[i] = 0;
		for(int j = 0; j < natts && j < MAX_COL; j++)
		{
			att_pos_array[i * MAX_COL + j] = 0;
			att_len_array[i * MAX_COL + j] = 0;
		}

		if(item[i].lp_flags != LP_NORMAL ||
		   item[i].lp_off < pd_lower ||
		   item[i].lp_len == 0 ||
		   (item[i].lp_off + item[i].lp_len) > PG_PAGE_SIZE)
		{
			continue;
		}

		parse_tuple_attr(i, natts, desc_len_in, desc_align_in, page, item[i].lp_off, item[i].lp_len,
				tup_off_array, tup_len_array,
				att_pos_array, att_len_array);
	}
}
//打印元组内容
// void print_tuple_content(unsigned char *page, unsigned short tupoff, unsigned short tuplen,
//                          unsigned short *att_pos_array, unsigned short *att_len_array, int i, int natts) {
//     // 假设每个tuple从tupoff开始，长度为tuplen
//     unsigned char *tuple_data = page + tupoff;
//     //printf("Hit tuple at position %d:\n", i);
//     // 打印tuple的前16个字节作为示例
//     for (int k = 0; k < 16 && k < tuplen; k++) {
//         printf("%02x ", tuple_data[k]);
//     }
//     printf("\n");
//     // 打印字段信息（假设att_pos_array和att_len_array有字段的偏移和长度）
//     for (int k = 0; k < natts; k++) {
//         unsigned short att_off = att_pos_array[i * MAX_COL + k];
//         unsigned short att_len = att_len_array[k];
//         // 字段从tuple的att_off开始，长度为att_len
//         unsigned char *att_data = tuple_data + att_off;
//         printf("Attribute %d (length %d bytes): ", k, att_len);
//         for (int m = 0; m < att_len && m < 16; m++) {
//             printf("%02x ", att_data[m]);
//         }
//         printf("\n");
//         // 如果字段是float类型，可以转换为double打印
//         if (att_len == sizeof(double)) {
//             double float_val = *(double *)att_data;
//             printf("As float: %f\n", float_val);
//         }
//         // 如果字段是int类型，可以转换为int打印
//         else if (att_len == sizeof(int)) {
//             int int_val = *(int *)att_data;
//             printf("As int: %d\n", int_val);
//         }
//         // 其他类型可以类似处理
//     }
// }

void filter_tuple(unsigned char buf[8192], int blk_dat_offset, int page_id, int natts,
		int filter_cnt, filter_t *filter,
		int page_tuple_cnt,
		unsigned short *tup_off_array,unsigned short *tup_len_array,
		unsigned short *att_pos_array,unsigned short *att_len_array,
		int hit_off[MAX_TUPLE_PER_PAGE],
		int *hit_count_out)//blk_dat_offset 可以删除
{
	unsigned char * page = (unsigned char *)buf;
	//pd_linp_t *item;
	unsigned short tuplen, tupoff;

	int hit_count = 0;

	double  float8_stack[64];
	int		int_stack[64];
	u8		bool_stack[64];
	u8		float8_stack_top;
	u8		int_stack_top;
	u8		bool_stack_top;
	int		expression_valid;

	double	*float8_ptr = (double *)buf;
	int		*int_ptr = (int*)buf;
	ItemIdData *item = (ItemIdData*)(buf + 24 );
	(void)blk_dat_offset;
	for(int i=0; i<page_tuple_cnt; i++)
	{
		//xil_printf("index : %d,tuple_flag: %d tup_off : %d tup_len: %d\r\n",i,item[i].lp_flags,item[i].lp_off,item[i].lp_len);
		//xil_printf("index : %d ")
		if(item[i].lp_flags != LP_NORMAL ||
		   tup_off_array[i] == 0 ||
		   tup_len_array[i] == 0 ||
		   (tup_off_array[i] + tup_len_array[i]) > PG_PAGE_SIZE)
		{
			FILTER_COMMON_TRACE("Invalid tuple item: index=%d flag=%d off=%d len=%d\r\n",
								i, item[i].lp_flags, tup_off_array[i], tup_len_array[i]);
			continue;
		}
		
		tupoff = tup_off_array[i];//元组偏移位置
		tuplen = tup_len_array[i];//元组长度
		(void)tuplen;
		float8_stack_top = 0;
		int_stack_top = 0;
		bool_stack_top = 0;
		expression_valid = 1;
		for(int j=0; j<filter_cnt; j++){
			pg_clause_op_t *op = &filter->clause[j];
			u16 op_class = op->op_class;
			if(T_OpExpr == op->op_type){
				int arg0_tag = op->arg0_tag;
				int arg0_index = op->arg0_index;
				int arg1_tag = op->arg1_tag;
				int arg1_index = op->arg1_index;

				if(C_FLOAT8 == op_class)
				{
					double arg0, arg1;
					if(T_Var == arg0_tag){
						if(arg0_index >= natts || arg0_index >= MAX_COL){
							expression_valid = 0;
							break;
						}
						unsigned short arg_off = *(att_pos_array + i*MAX_COL + arg0_index);
						arg_off += tupoff;//arg0 的
						arg_off = arg_off >> 3;
						arg0 = float8_ptr[arg_off];
					}else if(T_Const == arg0_tag){
						if(arg0_index >= 64){
							expression_valid = 0;
							break;
						}
						arg0 = filter->const_float8[arg0_index];
					}else{
						if(float8_stack_top == 0){
							expression_valid = 0;
							break;
						}
						float8_stack_top -= 1;
						unsigned short index = float8_stack_top;
						arg0 = float8_stack[index];
					}

					if(T_Var == arg1_tag)
					{
						if(arg1_index >= natts || arg1_index >= MAX_COL){
							expression_valid = 0;
							break;
						}
						unsigned short arg_off = *(att_pos_array + i*MAX_COL + arg1_index);
						arg_off += tupoff;
						arg_off = arg_off >> 3;
						arg1 = float8_ptr[arg_off];
					}else if(T_Const == arg1_tag){
						if(arg1_index >= 64){
							expression_valid = 0;
							break;
						}
						arg1 = filter->const_float8[arg1_index];
					}else{
						if(float8_stack_top == 0){
							expression_valid = 0;
							break;
						}
						float8_stack_top -= 1;
						unsigned short index = float8_stack_top;
						arg1 = float8_stack[index];
					}

					u16 func_id = op->func_id;
					switch(func_id){
					//float8 compare
					case FLOAT8_EQ:
						FLOAT8_CMP(==);
						break;
					case FLOAT8_NE:
						FLOAT8_CMP(!=);
						break;
					case FLOAT8_GE:
						FLOAT8_CMP(>=);
						break;
					case FLOAT8_LE:
						FLOAT8_CMP(<=);
						break;
					case FLOAT8_GT:
						FLOAT8_CMP(>);
						break;
					case FLOAT8_LT:
						FLOAT8_CMP(<);
						break;
						//float8 math
					case FLOAT8PL:
						FLOAT8_MATH(+);
						break;
					case FLOAT8MI:
						FLOAT8_MATH(-);
						break;
					case FLOAT8MUL:
						FLOAT8_MATH(*);
						break;
						//                    case FLOAT8DIV:
						//                    	FLOAT8_MATH(/);
						//                    	break;
					default:
						break;
					}
				}else if(C_INT == op_class){
					int arg0, arg1;
					if(T_Var == arg0_tag){
						if(arg0_index >= natts || arg0_index >= MAX_COL){
							expression_valid = 0;
							break;
						}
						unsigned short arg_off = *(att_pos_array + i*MAX_COL + arg0_index);
						arg_off += tupoff;
						arg_off = arg_off >> 2;
						arg0 = int_ptr[arg_off];
					}else if(T_Const == arg0_tag){
						if(arg0_index >= 128){
							expression_valid = 0;
							break;
						}
						arg0 = filter->const_int[arg0_index];
					}else{
						if(int_stack_top == 0){
							expression_valid = 0;
							break;
						}
						int_stack_top -= 1;
						unsigned short index = int_stack_top;
						arg0 = int_stack[index];
					}

					if(T_Var == arg1_tag){
						if(arg1_index >= natts || arg1_index >= MAX_COL){
							expression_valid = 0;
							break;
						}
						unsigned short arg_off = *(att_pos_array + i*MAX_COL + arg1_index);
						arg_off += tupoff;
						arg_off = arg_off >> 2;
						arg1 = int_ptr[arg_off];
					}else if(T_Const == arg1_tag){
						if(arg1_index >= 128){
							expression_valid = 0;
							break;
						}
						arg1 = filter->const_int[arg1_index];
					}else{
						if(int_stack_top == 0){
							expression_valid = 0;
							break;
						}
						int_stack_top -= 1;
						unsigned short index = int_stack_top;
						arg1 = int_stack[index];
					}

					u16 func_id = op->func_id;
					switch(func_id){
					//int compare
					case DATE_EQ:
					case INT4_EQ:
						INT_CMP(==);
						break;
					case DATE_NE:
					case INT4_NE:
						INT_CMP(!=);
						break;
					case DATE_GE:
					case INT4_GE:
						INT_CMP(>=);
						break;
					case DATE_LE:
					case INT4_LE:
						INT_CMP(<=);
						break;
					case DATE_GT:
					case INT4_GT:
						INT_CMP(>);
						break;
					case DATE_LT:
					case INT4_LT:
						INT_CMP(<);
						break;
					default:
						break;
					}
				}else if(C_STRING == op_class){
					const unsigned char *arg0 = NULL;
					const unsigned char *arg1 = NULL;
					unsigned short arg0_len = 0;
					unsigned short arg1_len = 0;

					if(T_Var == arg0_tag){
						if(arg0_index >= natts || arg0_index >= MAX_COL){
							expression_valid = 0;
							break;
						}
						unsigned short arg_off = *(att_pos_array + i*MAX_COL + arg0_index);
						arg0 = page + tupoff + arg_off;
						arg0_len = *(att_len_array + i*MAX_COL + arg0_index);
					}else if(T_Const == arg0_tag){
						if((unsigned int)arg0_index + op->arg0_len > sizeof(filter->const_char)){
							expression_valid = 0;
							break;
						}
						arg0 = (const unsigned char *)filter->const_char + arg0_index;
						arg0_len = op->arg0_len;
					}

					if(T_Var == arg1_tag){
						if(arg1_index >= natts || arg1_index >= MAX_COL){
							expression_valid = 0;
							break;
						}
						unsigned short arg_off = *(att_pos_array + i*MAX_COL + arg1_index);
						arg1 = page + tupoff + arg_off;
						arg1_len = *(att_len_array + i*MAX_COL + arg1_index);
					}else if(T_Const == arg1_tag){
						if((unsigned int)arg1_index + op->arg1_len > sizeof(filter->const_char)){
							expression_valid = 0;
							break;
						}
						arg1 = (const unsigned char *)filter->const_char + arg1_index;
						arg1_len = op->arg1_len;
					}

					if(arg0 == NULL || arg1 == NULL){
						bool_stack[bool_stack_top] = 0;
						bool_stack_top++;
					}else{
						int cmp = filter_memcmp_payload(arg0, arg0_len, arg1, arg1_len);
						u16 func_id = op->func_id;
						switch(func_id){
						case TEXT_EQ:
						case BPCHAR_EQ:
							bool_stack[bool_stack_top] = (cmp == 0) ? 1 : 0;
							bool_stack_top++;
							break;
						case TEXT_NE:
						case BPCHAR_NE:
							bool_stack[bool_stack_top] = (cmp != 0) ? 1 : 0;
							bool_stack_top++;
							break;
						case TEXT_LT:
						case BPCHAR_LT:
							bool_stack[bool_stack_top] = (cmp < 0) ? 1 : 0;
							bool_stack_top++;
							break;
						case TEXT_LE:
						case BPCHAR_LE:
							bool_stack[bool_stack_top] = (cmp <= 0) ? 1 : 0;
							bool_stack_top++;
							break;
						case TEXT_GT:
						case BPCHAR_GT:
							bool_stack[bool_stack_top] = (cmp > 0) ? 1 : 0;
							bool_stack_top++;
							break;
						case TEXT_GE:
						case BPCHAR_GE:
							bool_stack[bool_stack_top] = (cmp >= 0) ? 1 : 0;
							bool_stack_top++;
							break;
						default:
							bool_stack[bool_stack_top] = 0;
							bool_stack_top++;
							break;
						}
					}
				}else{
					expression_valid = 0;
					break;
				}
			}else if(T_BoolExpr == op->op_type){ //bool expr
				int nargs = op->nargs;
				if(nargs <= 0 || nargs > bool_stack_top ||
				   (op->func_id != OP_AND && op->func_id != OP_OR)){
					expression_valid = 0;
					break;
				}
				bool_stack_top -= nargs;
				int sum = 0;
				for(int i=0; i<nargs; i++){
					sum += bool_stack[bool_stack_top + i];
				}
				bool bool_ret;
				if(OP_AND == op->func_id){
					if(nargs == sum){
						bool_ret = 1;
					}else{
						bool_ret = 0;
					}
				}else{
					if(0 == sum){
						bool_ret = 0;
					}else{
						bool_ret = 1;
					}

				}
				bool_stack[bool_stack_top] = bool_ret;
				bool_stack_top++;
			}else{
				expression_valid = 0;
				break;
			}
		}//for(int j=0; j<filter_cnt; j++)

		/* A SQL scan without a WHERE clause must pass every valid tuple. */
		bool is_hit = expression_valid &&
			(filter_cnt == 0 || (bool_stack_top > 0 && bool_stack[0]));
		if(is_hit){
			if(hit_count >= MAX_TUPLE_PER_PAGE)
				break;
			hit_off[hit_count] = (page_id<<16)|i;
			// printf("filter\n");
			// printf("tupleoff: %d  tuple %d  is_true:%d \n",hit_off[hit_count],i,is_hit);

			hit_count++;
			
		}
	}//filter:for(int i=0; i<page_tuple_cnt; i++)

	*hit_count_out = hit_count;
}
