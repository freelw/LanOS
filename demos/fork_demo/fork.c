#include "mm.h"
#include "sched.h"

int copy_mem(int nr,struct task_struct * p)
{
	return 0;
}

int copy_process(long ebp,long edi,long esi,long gs,long none,
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
	p->pid = ++ last_pid;
	p->tss.esp0 = PAGE_SIZE + (long)(p);
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
	p->tss.gs = gs & 0xffff;
	task[last_pid] = p;

	//todo: 
	//copy mem

	if (copy_mem(last_pid, p)) {
		//todo:
		//abort
	}
	set_ldt_desc(gdt+last_pid+FIRST_LDT_ENTRY, &(p->ldt));
    return 0;
}