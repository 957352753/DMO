#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/memfd.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <linux/un.h>

#include "common.h"
#include "pg_custom_pathlist_hook.h"
#include "hlog.h"
#include "pg_filter_common.h"
#include "fpga_qual.h"
#include "query.h"
#include "share_mem.h"
#include "shmem_lib.h"
#include "msg_client.h"

#define max(a,b) ((a) > (b) ? (a) : (b))
#define FPGA_SERVER_PATH        "/home/cjj/module/seqscan_server"
#define PAGE_HEADER_SIZE 24

static void *g_share_mem = NULL;

static Plan*
PlanPGCustomScanPath (PlannerInfo* root,
					RelOptInfo* rel,
					CustomPath* best_path,
					List* tlist,
					List* clauses,
					List* custom_plans);

static Node*
CreatePGCustomScanState (CustomScan* custom_scan);

static void BeginPGCustomScan (CustomScanState* node, EState* estate, int eflags);
static void ReScanPGCustomScan (CustomScanState* node);
static TupleTableSlot* ExecPGCustomScan (CustomScanState* node);
static void EndPGCustomScan (CustomScanState* node);
static void iter_files_to_requset(qual_t *qual,int *filter_request_cnt, extent_info_t *filter_extents);
static void transfer_filepath_to_filter_extents(const char *filepath,int *filter_request_cnt, extent_info_t *filter_extents);
static void split_extent_into_filter_requests(
        uint64_t start_block,
        uint64_t block_count,
        int *filter_request_cnt,
        extent_info_t *filter_extents);
static int get_tuple_cnt(const char *data);
static CustomPathMethods	PGCustomScan_path_methods = {
	"PGCustomScan",			   /* CustomName */
	PlanPGCustomScanPath,		 /* PlanCustomPath */
#if PG_VERSION_NUM < 90600
	NULL,					   /* TextOutCustomPath */
#endif
};

static CustomScanMethods	PGCustomScan_scan_methods = {
	"PGCustomScan",			   /* CustomName */
	CreatePGCustomScanState,	  /* CreateCustomScanState */
#if PG_VERSION_NUM < 90600
	NULL,					   /* TextOutCustomScan */
#endif
};

static CustomExecMethods	PGCustomScan_exec_methods = {
	"PGCustomScan",			   /* CustomName */
	BeginPGCustomScan,			/* BeginCustomScan */
	ExecPGCustomScan,			 /* ExecCustomScan */
	EndPGCustomScan,			  /* EndCustomScan */
	ReScanPGCustomScan,		   /* ReScanCustomScan */
	NULL,					   /* MarkPosCustomScan */
	NULL,					   /* RestrPosCustomScan */
#if PG_VERSION_NUM >= 90600
	NULL,					   /* EstimateDSMCustomScan */
	NULL,					   /* InitializeDSMCustomScan */
	NULL,					   /* InitializeWorkerCustomScan */
#endif
	NULL,		  		/* ExplainCustomScan */
};

static int get_tuple_cnt(const char *data) {
    const uint8_t *base = (const uint8_t *)data;   // 指向第一页数据
    uint16_t pd_lower = ((uint16_t)base[13] << 8) | base[12]; // 解析 pd_lower（2 字节）

    int tuple_cnt = (pd_lower - 24) >> 2;
    if (tuple_cnt < 0) tuple_cnt = 0;

    return tuple_cnt;
}

// 将一个 extent 的 (start_block, block_count) 拆成多个，每 32 blocks 一个
static void split_extent_into_filter_requests(
        uint64_t start_block,
        uint64_t block_count,
        int *filter_request_cnt,
        extent_info_t *filter_extents)
{
    uint64_t cur = start_block;
    uint64_t remain = block_count;

    while (remain > 0) {
        uint64_t this_block_cnt = (remain > SPLIT_BLOCK_SIZE) ?
                                   SPLIT_BLOCK_SIZE : remain;

        filter_extents[*filter_request_cnt].start_block = cur;
        filter_extents[*filter_request_cnt].block_count = this_block_cnt;

        (*filter_request_cnt)++;

        cur += this_block_cnt;
        remain -= this_block_cnt;
    }
}
static void transfer_filepath_to_filter_extents(const char *filepath,int *filter_request_cnt, extent_info_t *filter_extents)
{
	DBG_INFO("split_file_to_request filepath=%s\n", filepath);
	//读取该文件对应的逻辑块
	int fd = open(filepath, O_RDONLY);
    if (fd < 0) 
	{
        perror("open");
        return -1;
    }
	size_t size = sizeof(struct fiemap) + MAX_EXTENTS * sizeof(struct fiemap_extent);
    struct fiemap *fiemap = (struct fiemap *)malloc(size);
    if (!fiemap) {
        perror("malloc");
        close(fd);
        return -1;
    }

    memset(fiemap, 0, size);
    fiemap->fm_start = 0;
    fiemap->fm_length = ~0ULL;
    fiemap->fm_flags = 0;
    fiemap->fm_extent_count = MAX_EXTENTS;

    if (ioctl(fd, FS_IOC_FIEMAP, fiemap) < 0) {
        perror("FS_IOC_FIEMAP");
        close(fd);
        free(fiemap);
        return -1;
    }

    int count = (fiemap->fm_mapped_extents < (uint32_t)MAX_EXTENTS) ?
                fiemap->fm_mapped_extents : MAX_EXTENTS;
    for (int i = 0; i < count; i++) {
        struct fiemap_extent *e = &fiemap->fm_extents[i];
        uint64_t start_block = e->fe_physical / CSD_BLOCK_SIZE;
        uint64_t block_count = e->fe_length / CSD_BLOCK_SIZE;
		DBG_INFO("extent %d: start_block=%lu, block_count=%lu\n", i, start_block, block_count);
		 // ✨ 调用拆分函数，每 32 blocks 划分一次
        split_extent_into_filter_requests(
            start_block,
            block_count,
            filter_request_cnt,
            filter_extents
        );
    }

    free(fiemap);
    close(fd);

}
static void iter_files_to_requset(qual_t *qual,int *filter_extents_cnt, extent_info_t *filter_extents)
{
	char filepath[512];
	snprintf(filepath, sizeof(filepath), "%s", qual->filepath);
	transfer_filepath_to_filter_extents(filepath,filter_extents_cnt,filter_extents);
	//表数据大的话会被分为多个文件
	for(int i=1; i<qual->file_cnt; i++){
		snprintf(filepath, sizeof(filepath), "%s.%d", qual->filepath, i);
		transfer_filepath_to_filter_extents(filepath,filter_extents_cnt,filter_extents);
	}
	DBG_INFO("filter_extents_cnt: %d\n", *filter_extents_cnt);
}

static Plan*
PlanPGCustomScanPath (PlannerInfo* root,
					RelOptInfo* rel,
					CustomPath* best_path,
					List* tlist,
					List* clauses,
					List* custom_plans)
{
	List*		   PGCustom_quals = best_path->custom_private;

	CustomScan*		cscan = (CustomScan *)newNode(sizeof(FpgaCustomScan), T_CustomScan);
        FpgaCustomScan*		fpga_cscan = (FpgaCustomScan*) cscan;


	DBG_INFO("trace\n");
	cscan->flags = best_path->flags;
	cscan->methods = &PGCustomScan_scan_methods;

	cscan->scan.scanrelid = rel->relid;
	cscan->scan.plan.targetlist = tlist;
	//cscan->scan.plan.qual = extract_actual_clauses (clauses, false);
	cscan->scan.plan.qual = NULL;
	cscan->custom_exprs = PGCustom_quals;

	fpga_cscan->rel = rel;

	return &cscan->scan.plan;
}

static Node*
CreatePGCustomScanState (CustomScan* custom_scan)
{
	FpgaCustomScan* 	fpga_cscan = (FpgaCustomScan*) custom_scan;
        PGCustomScanState* 	pg_css = (PGCustomScanState*) palloc0 (sizeof (PGCustomScanState));


	DBG_INFO("trace\n");

	memset(pg_css, 0 , sizeof(PGCustomScanState));
	NodeSetTag (pg_css, T_CustomScanState);
	pg_css->css.flags = custom_scan->flags;
	pg_css->css.methods = &PGCustomScan_exec_methods;
	pg_css->baserel = fpga_cscan->rel;

	create_fpga_filter(pg_css->baserel, &pg_css->filter, &pg_css->max_attr_index, &pg_css->filter_cnt);

	return (Node*)&pg_css->css;
}

// static void BeginPGCustomScan (CustomScanState* node, EState* estate, int eflags)
// {
//         PGCustomScanState  	*pg_css = (PGCustomScanState *) node;
// 	Relation        	rel = node->ss.ss_currentRelation;
// 	struct tupleDesc        *tupdesc = RelationGetDescr(rel);
// 	RelFileNode             *rd_node = &rel->rd_node;

// 	DBG_INFO("trace pg_css=%p\n", pg_css);

// 	create_tupdesc_for_fpga(tupdesc, &pg_css->tupdesc);

// 	pg_css->blk_cnt = RelationGetNumberOfBlocksInFork(rel, MAIN_FORKNUM);

//         pg_css->file_cnt = pg_css->blk_cnt / ((BlockNumber) RELSEG_SIZE);

// 	if( pg_css->blk_cnt % ((BlockNumber) RELSEG_SIZE) ){
// 		pg_css->file_cnt++;
// 	}

// 	snprintf(pg_css->filepath, sizeof(pg_css->filepath), "%s/base/%d/%d", get_current_dir_name(), rd_node->dbNode, rd_node->relNode);

// 	create_qual_for_fpga(&pg_css->qual, pg_css->filepath, pg_css->file_cnt, pg_css->max_attr_index, pg_css->filter_cnt);

// 	//to improve performance avoid page fault, so we use global variable here
// 	pg_css->share_mem = g_share_mem;

// 	pg_css->respond_cnt = 0;

// 	pg_css->blk_tuple_cnt = 0;

// 	pg_css->total_tuple_cnt = 0;

// 	pg_css->request_cnt = 0;

// }

static void BeginPGCustomScan (CustomScanState* node, EState* estate, int eflags)
{
        PGCustomScanState  	*pg_css = (PGCustomScanState *) node;
	Relation        	rel = node->ss.ss_currentRelation;
	struct tupleDesc        *tupdesc = RelationGetDescr(rel);
	RelFileNode             *rd_node = &rel->rd_node;

	DBG_INFO("trace pg_css=%p\n", pg_css);

	create_tupdesc_for_fpga(tupdesc, &pg_css->tupdesc);

	pg_css->blk_cnt = RelationGetNumberOfBlocksInFork(rel, MAIN_FORKNUM);

    pg_css->file_cnt = pg_css->blk_cnt / ((BlockNumber) RELSEG_SIZE);

	if( pg_css->blk_cnt % ((BlockNumber) RELSEG_SIZE) ){
		pg_css->file_cnt++;
	}

	snprintf(pg_css->filepath, sizeof(pg_css->filepath), "%s/base/%d/%d", get_current_dir_name(), rd_node->dbNode, rd_node->relNode);

	create_qual_for_fpga(&pg_css->qual, pg_css->filepath, pg_css->file_cnt, pg_css->max_attr_index, pg_css->filter_cnt);

	//to improve performance avoid page fault, so we use global variable here
	pg_css->share_mem = (char *)g_share_mem;

	pg_css->result_page_cnt = 0;

	pg_css->cur_page_tuple_cnt = 0;

	pg_css->cur_result_page_index = 0;

	pg_css->cur_result_tup_index = 0;

	pg_css->flag = 0;	

	pg_css->total_tuple_cnt = 0;

	pg_css->cur_filter_extents_index = 0;
	//DBG_INFO("pg_css->filepath : %s, file_cnt:%d",pg_css->filepath, pg_css->file_cnt)
	pg_css->filter_extents_cnt = 0;
	iter_files_to_requset(&pg_css->qual,&pg_css->filter_extents_cnt,pg_css->filter_extents);
	pg_css->nvme_total_time = 0.0;
}


static void ReScanPGCustomScan (CustomScanState* node)
{
	DBG_INFO("trace\n");
}


// static TupleTableSlot *
// AccessPGCustomScan(CustomScanState *node)
// {
//         PGCustomScanState  	*pg_css = (PGCustomScanState *) node;
//         TupleTableSlot 		*slot = node->ss.ss_ScanTupleSlot;
//         HeapTuple               tuple;
// 	int 			blk_index;
// 	char			*page;
// 	int			tup_pos;
// 	int 			page_id;
// 	int			hit_tuple_index;
// 	pd_linp_t		*item;

// 	// test_page(slot);
// 	// return NULL;// 执行结束
// 	//DBG_INFO("trace, pg_css=%p\n", pg_css);

// 	if(0 == pg_css->request_cnt){
// 		pg_css->conn = connect_to_server(FPGA_SERVER_PATH);
// 		pg_css->request_cnt = send_query_requset(pg_css->conn, &pg_css->tupdesc, &pg_css->filter, &pg_css->qual, BLK_CNT); 
// 		DBG_INFO("filepath=%s, file_cnt=%d, requset_cnt=%d, pg_css=%p\n", pg_css->filepath, pg_css->file_cnt, pg_css->request_cnt, pg_css);
// 	}

// 	if( (pg_css->request_cnt == pg_css->respond_cnt) && (0 == pg_css->blk_tuple_cnt) ){
// 		DBG_INFO("Scan end\n");
// 		close_connect(pg_css->conn);
// 		return NULL;
// 	}

// 	if( (0 == pg_css->respond_cnt) || (0 == pg_css->blk_tuple_cnt) ){
// 		blk_index = recv_query_result(pg_css->conn);

// 		pg_css->blk_index = blk_index;

// 		pg_css->mem_blk = &pg_css->share_mem->blk[blk_index];

//                 pthread_mutex_unlock(&pg_css->mem_blk->blk_mutex);

//                 pg_css->blk_tuple_cnt =  pg_css->mem_blk->tup_cnt;

//                 pg_css->total_tuple_cnt += pg_css->blk_tuple_cnt;

// 		pg_css->respond_cnt++;

// 		pg_css->tup_index = 0;
// 	}

// 	tup_pos = pg_css->mem_blk->tup_pos[pg_css->tup_index];

// 	page_id = (tup_pos>>16) & 0xffff;

// 	hit_tuple_index = (tup_pos&0xffff);

// 	page = pg_css->mem_blk->src[page_id].dat;

// 	item = (pd_linp_t*)(page + 24);

// 	item += hit_tuple_index;

// 	pg_css->c_tuple.t_data = (HeapTupleHeader)(page + item->lp_off);

//     pg_css->c_tuple.t_len  = item->lp_len;

// 	tuple =  &pg_css->c_tuple;

//     ExecStoreTuple(tuple, slot, InvalidBuffer, false);

// 	//DBG_INFO("blk_index=%d, blk_tuple_cnt=%d, top_pos=%d\n", pg_css->blk_index, pg_css->blk_tuple_cnt, tup_pos);

// 	//print_slot(slot);

// 	pg_css->blk_tuple_cnt--;
	
// 	pg_css->tup_index++;

//     return slot;
// 	// return NULL;
// }
static inline double get_time_sec()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts); // 纳秒级计时，不受系统时间改变影响
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
static TupleTableSlot *
AccessPGCustomScan(CustomScanState *node)
{
	PGCustomScanState  	*pg_css = (PGCustomScanState *) node;
	TupleTableSlot 		*slot = node->ss.ss_ScanTupleSlot;
	HeapTuple            tuple;
	int 			blk_index;
	char			*page;
	int			tup_pos;
	int 			page_id;
	int			hit_tuple_index;
	pd_linp_t		*item;

	// test_page(slot);
	// return NULL;
	//DBG_INFO("trace, pg_css=%p\n", pg_css);
	//printf("pg_css->filepath: %s\n", pg_css->filepath);
	
	if(pg_css->flag == 0)// 获取结果集状态  需要发送数据
	{  
		
		if(pg_css->cur_filter_extents_index ==0)//第一次先建立连接
		{
			DBG_INFO("开始连接中间件\n");
			pg_css->conn = connect_to_server(FPGA_SERVER_PATH); //建立连接
			if (pg_css->conn < 0) 
			{
				DBG_INFO("连接中间件失败，无法继续\n");
				return -1;  // 或其他错误
			}
			DBG_INFO("连接中间件成功，conn=%d\n", pg_css->conn);
		}
		
		// if(pg_css->cur_filter_extents_index >= 1)//测试 仅发送固定次数请求
		// {
		// 	close_connect(pg_css->conn);
		// 	pg_css->flag = 2; //结束
		// 	return NULL;
		// }



		int result_page_cnt = 0;//结果集页数
		while (!result_page_cnt && pg_css->cur_filter_extents_index < pg_css->filter_extents_cnt)
		{
			double start_ts = get_time_sec();

			//发送请求
			DBG_INFO("发送请求，当前请求索引 %d \n", pg_css->cur_filter_extents_index);
			send_query_requset(pg_css->conn, &pg_css->tupdesc, &pg_css->filter, &pg_css->qual,&pg_css->filter_extents[pg_css->cur_filter_extents_index]);
			pg_css->cur_filter_extents_index++;
			
			//return NULL; //测试使用

			recv_query_result(pg_css->conn,pg_css->pages);   //接收结果集
			memcpy(&result_page_cnt, pg_css->pages[0].data, sizeof(int));  //结果集第一页的前4个字节存放结果集页数
			DBG_INFO("发送请求后接收结果集，结果集页数为 %d\n", result_page_cnt);
			if(get_tuple_cnt(pg_css->pages[0].data) == 0 )
			{
				DBG_INFO("结果集第一页无元组，继续发送下一个请求\n");
				result_page_cnt = 0;
				continue;
			}
			// for(int i = 0; i < result_page_cnt; i++) 
			// {
			// 	DBG_INFO("recv_query_result: pages[%d] tuple_cnt=%d\n",i,get_tuple_cnt(pg_css->pages[i].data));
			// }
			double end_ts = get_time_sec();
			double duration = end_ts - start_ts;

			pg_css->nvme_total_time += duration;
			DBG_INFO("total_time=%.6f s\n",pg_css->nvme_total_time);
			//return NULL;//测试
		}
		if(result_page_cnt == 0)//没有结果集了
		{
			pg_css->flag = 2; //结束
		}
		else
		{
			pg_css->result_page_cnt = result_page_cnt;
			pg_css->cur_result_page_index = 0;
			pg_css->cur_result_tup_index = 0;
			pg_css->cur_page_tuple_cnt = get_tuple_cnt(pg_css->pages[pg_css->cur_result_page_index].data);
			pg_css->flag = 1; //处理结果集状态
		}
	}
	if(pg_css->flag == 1)
	{
		//当前页的指针 取出对应的元组
		char *page = pg_css->pages[pg_css->cur_result_page_index].data;

		item = (pd_linp_t*)(page + 24);

		item += pg_css->cur_result_tup_index;

		pg_css->c_tuple.t_data = (HeapTupleHeader)(page + item->lp_off);

        pg_css->c_tuple.t_len  = item->lp_len;

		tuple =  &pg_css->c_tuple;
		//DBG_INFO("处理结果集页面 %d 中的元组 %d  当前页共有%d元组\n", pg_css->cur_result_page_index, pg_css->cur_result_tup_index,pg_css->cur_page_tuple_cnt);
		
		//DBG_INFO("item->lp_off : %d  item->lp_len : %d item->lp_flags: %d\n", item->lp_off, item->lp_len,item->lp_flags);
        ExecStoreTuple(tuple, slot, InvalidBuffer, false);
		//print_slot(slot);
		
		pg_css->total_tuple_cnt ++;
		pg_css->cur_result_tup_index ++;
		//判断元组是否超出范围
		if (pg_css->cur_result_tup_index >= pg_css->cur_page_tuple_cnt)
		{
			/* 进入下一页 */
			pg_css->cur_result_page_index++;
			
			if (pg_css->cur_result_page_index >= pg_css->result_page_cnt)//当前页面 是否超出页面结果集范围  如果超出 进入获取结果集状态
			{
				/* 所有结果页处理完毕 → 回到获取结果集状态 */
				pg_css->flag = 0;
			}
			else/* 切换到新页面并重置 tuple 索引 */
			{
				pg_css->cur_result_tup_index = 0;// 重置当前元组索引
				pg_css->cur_page_tuple_cnt = get_tuple_cnt(pg_css->pages[pg_css->cur_result_page_index].data);
			}
		}
	}
	if(pg_css->flag == 2)//结束
	{
		DBG_INFO("Scan end\n");
		close_connect(pg_css->conn);
		return NULL;
	}
	//return NULL;
	return slot;

	
}
void test_page(TupleTableSlot *slot)
{
	#define PAGE_SIZE 8192
	#define MAX_PAGES 1
	#define PAGE_HEADER_SIZE 24
	#ifndef max
	#define max(a,b) ((a) > (b) ? (a) : (b))
	#endif
	FILE *fp1 = fopen("/home/cjj/module/middle_ware/normal.dat", "rb");
	FILE *fp2 = fopen("/home/cjj/module/middle_ware/filter.dat", "rb");

    if (fp1 == NULL || fp2 == NULL) 
    {
        perror("无法打开文件");
        return -1;
    }

    // 读取文件内容
    char buffer1[PAGE_SIZE * MAX_PAGES];
    size_t bytesRead = fread(buffer1, 1, sizeof(buffer1), fp1);
    if (bytesRead == 0) {
        perror("读取文件失败");
        fclose(fp1);
        return 1;
    }

	char buffer2[PAGE_SIZE * MAX_PAGES];
    bytesRead = fread(buffer2, 1, sizeof(buffer2), fp2);
    if (bytesRead == 0) {
        perror("读取文件失败");
        fclose(fp2);
        return 1;
    }


    for(int i = 0; i < MAX_PAGES; i++)
    {
		printf("normal.dat \n");
        char *page = buffer1 + i * PAGE_SIZE;
        PageHeaderData *hdr = (PageHeaderData *) page;
        uint16_t pd_lower = hdr->pd_lower;
        int tuple_cnt = max((pd_lower - PAGE_HEADER_SIZE) >> 2, 0);
        printf("第 %d 页中 tuple 的数量为: %d\n", i, tuple_cnt);
		for (int j = 0; j < tuple_cnt; j++)
		{
			pd_linp_t *item = (pd_linp_t *)(page + PAGE_HEADER_SIZE) + j;
			printf("  tuple %d: offset = %u, length = %u\n", j, item->lp_off, item->lp_len);
			// 打印tuple内容
			HeapTupleData tuple;
			tuple.t_data = (HeapTupleHeader)(page + item->lp_off);
			tuple.t_len = item->lp_len;
			ExecStoreTuple(&tuple, slot, InvalidBuffer, false);
			print_slot(slot);
			if(j > 10)
			{
				break;
			}
		}
		

		printf("filter.dat \n");
		page = buffer2 + i * PAGE_SIZE;
        hdr = (PageHeaderData *) page;
        pd_lower = hdr->pd_lower;
        tuple_cnt = max((pd_lower - PAGE_HEADER_SIZE) >> 2, 0);
        printf("第 %d 页中 tuple 的数量为: %d\n", i, tuple_cnt);
		for (int j = 0; j < tuple_cnt; j++)
		{
			pd_linp_t *item = (pd_linp_t *)(page + PAGE_HEADER_SIZE) + j;
			printf("  tuple %d: offset = %u, length = %u\n", j, item->lp_off, item->lp_len);
			// 打印tuple内容
			HeapTupleData tuple;
			tuple.t_data = (HeapTupleHeader)(page + item->lp_off);
			tuple.t_len = item->lp_len;
			ExecStoreTuple(&tuple, slot, InvalidBuffer, false);
			print_slot(slot);
			if(j > 10)
			{
				break;
			}
		}
    }
    fclose(fp1);
	fclose(fp2);
}

static bool
RecheckPGCustomScan(CustomScanState *node, TupleTableSlot *slot)
{
        return true;
}

static TupleTableSlot *
ExecPGCustomScan(CustomScanState *node)
{
        return ExecScan(&node->ss,
                                        (ExecScanAccessMtd) AccessPGCustomScan,
                                        (ExecScanRecheckMtd) RecheckPGCustomScan);
}


static void EndPGCustomScan (CustomScanState* node)
{
        PGCustomScanState  *pg_css = (PGCustomScanState *) node;
	DBG_INFO("trace\n");

	//munmap(pg_css->share_mem, sizeof(share_mem_data_t));

	close_connect(pg_css->conn);

	DBG_INFO("trace end\n");
}

void PGSetCustomRelPathlistHook (PlannerInfo* root, RelOptInfo* baserel,
				   Index rtindex, RangeTblEntry* rte)
{

	char            relkind;
	int		baserestrictinfo_cnt;
        ListCell*       lc;
        CustomPath* cpath;
        Relids    required_outer;

        DBG_INFO("set custom hook\n");

        if (rte->rtekind != RTE_RELATION) {
                DBG_INFO("return rte->rtekind != RTE_RELATION, %d, %d\n", rte->rtekind, RTE_RELATION);
                return;
        }

        relkind = get_rel_relkind (rte->relid);

        if (relkind != RELKIND_RELATION &&
                relkind != RELKIND_MATVIEW &&
                relkind != RELKIND_TOASTVALUE) {
                DBG_INFO("return rte->rtekind != RTE_RELATION, %d, %d\n", rte->rtekind, RTE_RELATION);
                return;
        }


	baserestrictinfo_cnt = list_length(baserel->baserestrictinfo);
	if( 0 == baserestrictinfo_cnt){
		DBG_INFO("baserestrictinfo_cnt=0 , query not support\n");
		return ;
	}

	foreach (lc, baserel->baserestrictinfo) {
                RestrictInfo* rinfo = (RestrictInfo*) lfirst (lc);
                int     is_support;

                if (!IsA (rinfo, RestrictInfo)) {
                    continue;
                }
                is_support = 1;
                fpga_ops_check((Node*)rinfo->clause, &is_support);
                if(0 == is_support){
                        DBG_INFO("query not support\n");
                        return ;
                }
        }

	required_outer = baserel->lateral_relids;

	cpath = (CustomPath*) palloc0 (sizeof (CustomPath));
	cpath->path.type = T_CustomPath;
	cpath->path.pathtype = T_CustomScan;
	cpath->path.parent = baserel;
#if PG_VERSION_NUM >= 90600
	cpath->path.pathtarget = baserel->reltarget;
#endif
	cpath->path.param_info = get_baserel_parampathinfo (root, baserel, required_outer);

	cpath->flags = CUSTOMPATH_SUPPORT_BACKWARD_SCAN;
	cpath->custom_private = NULL;
	cpath->methods = &PGCustomScan_path_methods;

	DBG_INFO("add_path for rel, relid=%d, reltablespace=%d\n", baserel->relid, baserel->reltablespace);

	if(!g_share_mem){
		DBG_INFO("open_share_memory\n");
		// g_share_mem = open_share_memory(SHARE_MEM_NAME, sizeof(share_mem_data_t));
		g_share_mem = open_share_memory(SHARE_MEM_NAME, 32 * 1024);       //将shared memory映射到本进程空间，大小改为32K
		DBG_INFO("共享内存地址: %p\n", g_share_mem);
	}

	add_path (baserel, &cpath->path);
}

