#include "mm.h"

#define PAGEING_PAGES (63*1024*1024)/(4*1024)

unsigned char mem_map[PAGEING_PAGES];
#define RESERVE_PAGES 896

extern void assign_cr3_cr0(unsigned long);

void setup_paging()
{
	unsigned long *pg_dir = PAGE_DIR; //lan_os最多不能超过8k
	unsigned long *pg[PG_NUM]; //虚拟机必须给64M以上的物理内存
	pg[0] = (unsigned long)(pg_dir) + 0x1000;
	for (int i = 1; i < PG_NUM; ++ i) {
		pg[i] = (unsigned long)pg[i-1] + 0x1000;
	}
	for (int i = 0; i < PG_NUM; ++ i) {
		pg_dir[i] = (unsigned long)(pg[i]) + 7;
	}
	for (int i = 0; i < 1024*PG_NUM; ++ i) {
		pg[0][i] = (i << 12) + 7;
	}
	assign_cr3_cr0(pg_dir);
}

void init_mem_map()
{
    for (int i = 0; i < PAGEING_PAGES; ++ i) {
        mem_map[i] = 0;
    }
    for (int i = 0; i < RESERVE_PAGES; ++ i) {
        mem_map[i] = 100;
    }
}

void mm_init()
{
    init_mem_map();
    setup_paging();
}

unsigned long get_free_page()
{
    for (int i = PAGEING_PAGES-1; i >= 0; -- i) {
        if (0 == mem_map[i]) {
            mem_map[i] = 1;
            return LOW_MEM+i*4096;
        }
    }
    return 0;
}

int copy_page_tables(unsigned long from,unsigned long to,long size)
{
    return 0;
}