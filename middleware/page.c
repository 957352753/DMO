#include "page.h"
MemPageWriter writer;
char result_buf[PAGE_SIZE * 16];

void init_mem_page(MemPageWriter *w, char *mem_base)
{
    w->base_addr = mem_base;
    w->cur_page_index = 0;
    w->tuple_count = 0;
    w->free_lower = PAGE_HEADER_SIZE;
    w->free_upper = PAGE_SIZE;
    //DBG_INFO("init_mem_page base_addr=%p\n",w->base_addr);
    // 初始化第一页
    memset(w->base_addr, 0, PAGE_SIZE);

    PageHeaderData *hdr = (PageHeaderData *)(w->base_addr);
    hdr->pd_lower = PAGE_HEADER_SIZE;
    hdr->pd_upper = PAGE_SIZE;
    hdr->pd_special = PAGE_SIZE;
}


int switch_to_next_page(MemPageWriter *w)
{
    if (w->cur_page_index + 1 >= MAX_PAGES)
    {
        //xil_printf("No more space: all %d pages full", MAX_PAGES);
        printf("No more space: all %d pages full", MAX_PAGES);
        return -1;
    }

    w->cur_page_index++;
    w->tuple_count = 0;
    w->free_lower = PAGE_HEADER_SIZE;
    w->free_upper = PAGE_SIZE;

    char *page = w->base_addr + w->cur_page_index * PAGE_SIZE;
    memset(page, 0, PAGE_SIZE);

    PageHeaderData *hdr = (PageHeaderData *) page;
    hdr->pd_lower = PAGE_HEADER_SIZE;
    hdr->pd_upper = PAGE_SIZE;
    hdr->pd_special = PAGE_SIZE;
    return 0;
}


int append_tuple_to_mem_page(MemPageWriter *w, HeapTuple tuple) // HeapTupleData 只使用到 t_len 和 t_data
{
    uint16_t tlen = tuple->t_len;
    uint16_t aligned_tlen = MAXALIGN(tlen);

    char *page = w->base_addr + w->cur_page_index * PAGE_SIZE;
    PageHeaderData *hdr = (PageHeaderData *) page;

    uint16_t needed = sizeof(ItemIdData) + aligned_tlen;
    if (w->free_upper - w->free_lower < needed)
    {
        //printf("Current page %d full, need to switch page\n", w->cur_page_index);
        //printf("Total tuples in current page: %d\n", w->tuple_count);
        // 当前页放不下了，切换新页
        if (switch_to_next_page(w) < 0)
        {
            //printf("Failed to switch to next page\n");
            return -1 ;  // 切换页失败
        }
            
        page = w->base_addr + w->cur_page_index * PAGE_SIZE;
        hdr = (PageHeaderData *) page;
    }

    // 计算line pointer位置
    ItemIdData *item = (ItemIdData *)(page + w->free_lower);
    w->free_lower += sizeof(ItemIdData);

    // tuple放在free_upper下方
    w->free_upper -= aligned_tlen;

    // 拷贝tuple内容
    memcpy(page + w->free_upper, tuple->t_data, tlen);

    // 设置line pointer
    item->lp_off = w->free_upper;
    item->lp_len = tlen;
    item->lp_flags = LP_NORMAL;

    // 更新页头
    hdr->pd_lower = w->free_lower;
    hdr->pd_upper = w->free_upper;
    w->tuple_count++;
    return 0;   
}