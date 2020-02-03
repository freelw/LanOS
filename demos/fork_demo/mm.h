#ifndef MM_H
#define MM_H

#define PAGE_DIR 0x8000
#define PG_NUM 16

extern void mm_init();
extern void *get_free_page();

#endif