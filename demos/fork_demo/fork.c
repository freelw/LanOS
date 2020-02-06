#include "mm.h"
#include "sched.h"

extern void first_return_from_kernel();

int copy_mem(int nr,struct task_struct * p)
{
	unsigned long old_data_base,new_data_base,data_limit;
	unsigned long old_code_base,new_code_base,code_limit;

	code_limit=get_limit(0x0f);
	data_limit=get_limit(0x17);
	old_code_base = get_base(current->ldt[1]);
	old_data_base = get_base(current->ldt[2]);
	new_data_base = new_code_base = nr * TASK_SIZE + PG_NUM*4*1024*1024;
	p->start_code = new_code_base;
	set_base(&(p->ldt[1]),new_code_base);
	set_base(&(p->ldt[2]),new_data_base);
	if (copy_page_tables(old_data_base,new_data_base,data_limit)) {
		// todo
		// free_page_tables
	}
	return 0;
}

int find_empty_process()
{
	return ++ last_pid;
}

int copy_process(long eax, long ebp,long edi,long esi,long gs,long none,
		long ebx,long ecx,long edx, long orig_eax, 
		long fs,long es,long ds,
		long eip,long cs,long eflags,long esp,long ss)
{
    struct task_struct *p = (struct task_struct *)get_free_page();

	if (!p) {
		// todo
	}
	*p = *current;
	p->state = TASK_UNINTERRUPTIBLE;
	p->pid = eax;
	/*p->tss.esp0 = PAGE_SIZE + (long)(p);
	p->tss.ss0 = 0x10;
	p->tss.eip = eip;
	p->tss.eflags = eflags;
	p->tss.eax = 0;
	p->tss.ecx = ecx;
	p->tss.edx = edx;
	p->tss.ebx = ebx;
	p->tss.esp = esp;
	p->tss.ebp = ebp;
	p->tss.esi = esi;
	p->tss.edi = edi;
	p->tss.es = es & 0xffff;
	p->tss.cs = cs & 0xffff;
	p->tss.ss = ss & 0xffff;
	p->tss.ds = ds & 0xffff;
	p->tss.fs = fs & 0xffff;
	p->tss.gs = gs & 0xffff;*/
	task[last_pid] = p;
	p->kernel_stack = PAGE_SIZE + (long)(p);
	*((unsigned long *)--(p->kernel_stack)) = ss;
	*((unsigned long *)--(p->kernel_stack)) = esp;
	*((unsigned long *)--(p->kernel_stack)) = eflags;
	*((unsigned long *)--(p->kernel_stack)) = cs;
	*((unsigned long *)--(p->kernel_stack)) = eip;
	*((unsigned long *)--(p->kernel_stack)) = first_return_from_kernel;
	*((unsigned long *)--(p->kernel_stack)) = 0; // eax
	*((unsigned long *)--(p->kernel_stack)) = ebx;
	*((unsigned long *)--(p->kernel_stack)) = ecx;
	*((unsigned long *)--(p->kernel_stack)) = edx;
	*((unsigned long *)--(p->kernel_stack)) = ebp;
	*((unsigned long *)--(p->kernel_stack)) = esi;
	*((unsigned long *)--(p->kernel_stack)) = edi;
	*((unsigned long *)--(p->kernel_stack)) = es & 0xffff;
	*((unsigned long *)--(p->kernel_stack)) = ds & 0xffff;
	*((unsigned long *)--(p->kernel_stack)) = fs & 0xffff;
	*((unsigned long *)--(p->kernel_stack)) = gs & 0xffff;
	if (copy_mem(last_pid, p)) {
		//todo:
		//abort
	}
	set_ldt_desc(gdt+last_pid+FIRST_LDT_ENTRY, &(p->ldt));
	p->state = TASK_RUNNING;
    return 1;
}