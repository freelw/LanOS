#include "mm.h"
#include "sched.h"

extern void panic(char *msg);

extern void first_return_from_kernel();
unsigned long get_data_base(struct task_struct *s)
{
	unsigned long ret = (s->ldt[2].a) >> 16;
	ret |= (s->ldt[2].b & 0xff) << 16;
	ret |= s->ldt[2].b & 0xff000000;
	return ret;
}

int copy_mem(int nr,struct task_struct * p)
{
	unsigned long old_data_base,new_data_base,data_limit;
	unsigned long new_code_base,code_limit;

	code_limit=get_limit(0x0f);
	data_limit=get_limit(0x17);
	old_data_base = get_data_base(current);
	new_data_base = new_code_base = nr * TASK_SIZE + PG_NUM*4*1024*1024;
	p->start_code = new_code_base;
	set_base(&(p->ldt[1]),new_code_base);
	set_base(&(p->ldt[2]),new_data_base);
	if (copy_page_tables(old_data_base,new_data_base,data_limit)) {
		// todo
		// free_page_tables
		panic("copy page tables failed.");
	}
	return 0;
}

int find_empty_process()
{
	for (int i = 0; i < NR_TASKS; ++ i) {
		if (!task_used[i]) {
			task_used[i] = 1;
			return i;
		}
	}
	return -1;
}

int copy_process(long eax, long ebp,long edi,long esi,long gs,long none,
		long ebx,long ecx,long edx, long orig_eax, 
		long fs,long es,long ds,
		long eip,long cs,long eflags,long esp,long ss)
{
	if (eax < 0) {
		panic("no emtpy task.");
	}
    struct task_struct *p = (struct task_struct *)get_free_page();

	if (!p) {
		// todo
	}
	*p = *current;
	p->state = TASK_UNINTERRUPTIBLE;
	p->pid = eax;
	task[eax] = p;
	long * krnstack = (long *)(PAGE_SIZE+(long)p);
	*(--krnstack) = ss;
	*(--krnstack) = esp;
	*(--krnstack) = eflags;
	*(--krnstack) = cs;
	*(--krnstack) = eip;
	*(--krnstack) = first_return_from_kernel;
	*(--krnstack) = 0; // eax
	*(--krnstack) = ebx;
	*(--krnstack) = ecx;
	*(--krnstack) = edx;
	*(--krnstack) = ebp;
	*(--krnstack) = esi;
	*(--krnstack) = edi;
	*(--krnstack) = es & 0xffff;
	*(--krnstack) = ds & 0xffff;
	*(--krnstack) = fs & 0xffff;
	*(--krnstack) = gs & 0xffff;
	p->kernel_stack = krnstack;
	if (copy_mem(eax, p)) {
		//todo:
		//abort
	}
	set_ldt_desc(gdt+eax+FIRST_LDT_ENTRY, &(p->ldt));
	p->state = TASK_RUNNING;
    return 1;
}
