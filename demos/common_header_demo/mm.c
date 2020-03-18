#include "mm.h"
#include "sched.h"

#define PAGEING_PAGES (63 * 1024 * 1024) / (4 * 1024)

unsigned char mem_map[PAGEING_PAGES];
#define RESERVE_PAGES 896

extern void assign_cr3_cr0(unsigned long);
extern void print_str(char *s);
extern void print_num(int num);
extern void panic(char *msg);
extern void print_hex(int num);

void setup_paging() {
  unsigned long *pg_dir = PAGE_DIR;  // lan_os最多不能超过0x8000 64个扇区
  unsigned long *pg[PG_NUM];  //虚拟机必须给64M以上的物理内存
  pg[0] = (unsigned long)(pg_dir) + 0x1000;
  for (int i = 1; i < PG_NUM; ++i) {
    pg[i] = (unsigned long)pg[i - 1] + 0x1000;
  }
  for (int i = 0; i < PG_NUM; ++i) {
    pg_dir[i] = (unsigned long)(pg[i]) + 7;
  }
  for (int i = 0; i < 1024 * PG_NUM; ++i) {
    pg[0][i] = (i << 12) + 7;
  }
  assign_cr3_cr0(pg_dir);
}

void init_mem_map() {
  for (int i = 0; i < PAGEING_PAGES; ++i) {
    mem_map[i] = 0;
  }
  for (int i = 0; i < RESERVE_PAGES; ++i) {
    mem_map[i] = 100;
  }
}

void mm_init() {
  init_mem_map();
  setup_paging();
}

unsigned long get_free_page() {
  for (int i = PAGEING_PAGES - 1; i >= 0; --i) {
    if (0 == mem_map[i]) {
      mem_map[i] = 1;
      unsigned char *free_page = LOW_MEM + i * 4096;
      for (int j = 0; j < 4096; ++j) {
        free_page[j] = 0;
      }
      return LOW_MEM + i * 4096;
    }
  }
  return 0;
}

void free_page(unsigned long addr) {
  if (addr < LOW_MEM) {
    return;
  }
  addr -= LOW_MEM;
  addr >>= 12;
  if (mem_map[addr]--) {
    return;
  }
  mem_map[addr] = 0;
  panic("trying to free free page");
}

int free_page_tables(unsigned long from, unsigned long size) {
  if (from & 0x3fffff) panic("free_page_tables called with wrong alignment");
  if (!from) panic("Trying to free up swapper memory space");
  size = (size + 0x3fffff) >> 22;
  unsigned long *dir = (unsigned long *)(((from >> 20) & 0xffc) + PAGE_DIR);
  for (; size-- > 0; dir++) {
    if (!(1 & *dir)) continue;
    unsigned long *pg_table = (unsigned long *)(0xfffff000 & *dir);
    for (unsigned long nr = 0; nr < 1024; nr++) {
      if (*pg_table) {
        if (1 & *pg_table) {
          free_page(0xfffff000 & *pg_table);
        } else {
          // swap_free(*pg_table >> 1);
          panic("swap free");
        }
        *pg_table = 0;
      }
      pg_table++;
    }
    free_page(0xfffff000 & *dir);
    *dir = 0;
  }
  invalidate();
  return 0;
}

int copy_page_tables(unsigned long from, unsigned long to, long size) {
  unsigned long *from_page_table;
  unsigned long *to_page_table;
  unsigned long this_page;
  unsigned long *from_dir, *to_dir;
  unsigned long new_page;
  unsigned long nr;

  if ((from & 0x3fffff) || (to & 0x3fffff)) {
    panic("copy page talbes error 0");
  }
  from_dir =
      (unsigned long
           *)(((from >> 20) & 0xffc) +
              PAGE_DIR); /* _pg_dir 和linux0.12定义不同，我们是从0x8000开始 */
  to_dir = (unsigned long *)(((to >> 20) & 0xffc) + PAGE_DIR);
  size = ((unsigned)(size + 0x3fffff)) >> 22;
  for (; size-- > 0; from_dir++, to_dir++) {
    from_page_table = (unsigned long *)(0xfffff000 & *from_dir);
    if (!(to_page_table = (unsigned long *)get_free_page())) {
      panic("copy page talbes error 1");
    }
    *to_dir = ((unsigned long)to_page_table) | 7;
    nr = 1024;
    for (; nr-- > 0; from_page_table++, to_page_table++) {
      this_page = *from_page_table;
      this_page &= ~2;
      *to_page_table = this_page;
      if (this_page > LOW_MEM) {
        *from_page_table = this_page;
        this_page -= LOW_MEM;
        this_page >>= 12;
        mem_map[this_page]++;
      }
    }
  }
  invalidate();
  return 0;
}

void copy_page(unsigned long old_page, unsigned long new_page) {
  old_page = old_page & 0xfffff000;
  new_page = new_page & 0xfffff000;
  for (int i = 0; i < 4096; ++i) {
    ((unsigned char *)new_page)[i] = ((unsigned char *)old_page)[i];
  }
}

void un_wp_page(unsigned long *table_entry) {
  unsigned long old_page = 0xfffff000 & *table_entry;
  if (old_page >= LOW_MEM && mem_map[MAP_NR(old_page)] == 1) {
    *table_entry |= 2;
    invalidate();
    return;
  }
  unsigned long new_page = get_free_page();
  if (!new_page) {
    panic("un_wp_page error 0");
  }
  if (old_page >= LOW_MEM) {
    mem_map[MAP_NR(old_page)]--;
  }
  copy_page(old_page, new_page);
  *table_entry = new_page | 7;
  invalidate();
}

unsigned long put_page(unsigned long page, unsigned long address) {
  if (page < LOW_MEM) panic("Trying to put page LOW_MEM");
  if (mem_map[(page - LOW_MEM) >> 12] != 1) panic("mem_map disagrees.");
  unsigned long page_table_index = (address >> 22) & 0x3ff;
  unsigned long *page_table =
      (unsigned long *)(page_table_index * 4 + PAGE_DIR);
  if ((*page_table) & 1) {
    page_table = (unsigned long *)(0xfffff000 & *page_table);
  } else {
    unsigned long tmp;
    if (!(tmp = get_free_page())) {
      return 0;
    }
    *page_table = tmp | 7;
    page_table = (unsigned long *)tmp;
  }
  page_table[(address >> 12) & 0x3ff] = page | 7;
  return page;
}

void get_empty_page(unsigned long address) {
  unsigned long tmp;

  if (!(tmp = get_free_page()) || !put_page(tmp, address)) {
    panic("get empty page error.");
  }
}

void do_no_page(unsigned long address) {
  address &= 0xfffff000;
  unsigned long tmp;
  unsigned long page_table_index = (address >> 22) & 0x3ff;
  unsigned long page = *(unsigned long *)(page_table_index * 4 + PAGE_DIR);
  if (page & 1) {
    page &= 0xfffff000;
    page += (address >> 10) & 0xffc;
    tmp = *(unsigned long *)(page);
    if (tmp && !(1 & tmp)) {
      panic("do no page swap.");
    }
  }
  tmp = address - current->start_code;
  if (tmp >= current->end_data) {
    get_empty_page(address);
    return;
  }
  if (!(page = get_free_page())) {
    panic("do no page get free page error.");
  }
  int FourKnum = tmp / 4096;
  bread(page, FourKnum, current->fs_index);
  if (!put_page(page, address)) {
    panic("do no page put page error.");
  }
  invalidate();
}

void e14(unsigned long address, unsigned long error_code) {
  if (error_code & 1) {
    unsigned long page_index = (address >> 12) & 0x3ff;
    unsigned long page_table_index = (address >> 22) & 0x3ff;
    unsigned long *table_entry =
        page_index * 4 +
        (0xfffff000 & (*(unsigned long *)(page_table_index * 4 + PAGE_DIR)));
    un_wp_page(table_entry);
  } else {
    do_no_page(address);
  }
}