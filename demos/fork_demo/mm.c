#include "mm.h"

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

void mm_init()
{
    setup_paging();
}

void *get_free_page()
{
    
}