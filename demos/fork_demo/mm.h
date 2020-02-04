#ifndef MM_H
#define MM_H

#define PAGE_SIZE 4096
#define PAGE_DIR 0x8000
#define PG_NUM 16

#define LOW_MEM 0x100000
#define invalidate() \
__asm__("movl %%eax,%%cr3"::"a" (0))

extern void mm_init();
extern unsigned long get_free_page();
extern int copy_page_tables(unsigned long from,unsigned long to,long size);

#endif