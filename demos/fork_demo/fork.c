#include "sched.h"

void *get_free_page(){}
int copy_process(long ebp,long edi,long esi,long gs,long none,
		long ebx,long ecx,long edx, long orig_eax, 
		long fs,long es,long ds,
		long eip,long cs,long eflags,long esp,long ss)
{
    struct task_struct *p = (struct task_struct *)get_free_page();
    return 0;
}