#include "sched.h"

struct task_struct * task[NR_TASKS];
struct task_struct init_task;
struct task_struct *current = &init_task;
long last_pid = -1;

void sched_init()
{
    init_task.state = TASK_UNINTERRUPTIBLE;
    init_task.pid = ++ last_pid;
    init_task.ldt[0].a = 0;
    init_task.ldt[0].b = 0;
    init_task.ldt[1].a = 0x3ff;
    init_task.ldt[1].b = 0xc0fa00;
    init_task.ldt[2].a = 0x3ff;
    init_task.ldt[2].b = 0xc0f200;
    set_ldt_desc(gdt+0+FIRST_LDT_ENTRY, &(init_task.ldt));
    task[0] = &init_task;
    lldt(0);
}


