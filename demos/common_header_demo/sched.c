#include "sched.h"
#include "mm.h"

struct task_struct *task[NR_TASKS];
int task_used[NR_TASKS];
struct task_struct init_task;
struct task_struct *current = (struct task_struct *)&init_task;
long last_sched_i = 0;

extern void switch_to(unsigned long, struct task_struct *);
extern void set_tss0_esp0(unsigned long);
extern void get_esp0_when_switch(unsigned long *);
extern void krn_stk0();

void sched_init() {
  for (int i = 0; i < NR_TASKS; ++i) {
    task[i] = 0;
    task_used[i] = 0;
  }
  task_used[0] = 1;
  init_task.state = TASK_UNINTERRUPTIBLE;
  init_task.pid = 0;
  init_task.ldt[0].a = 0;
  init_task.ldt[0].b = 0;
  init_task.ldt[1].a = 0x3ff;
  init_task.ldt[1].b = 0xc0fa00;
  init_task.ldt[2].a = 0x3ff;
  init_task.ldt[2].b = 0xc0f200;
  set_ldt_desc(gdt + 0 + FIRST_LDT_ENTRY, &(init_task.ldt));
  task[0] = &init_task;
  lldt(0);
  init_task.state = TASK_RUNNING;
}

void schedule() {
  int i = last_sched_i;
  while (1) {
    ++i;
    i %= NR_TASKS;
    if (task[i]) {
      if (task[i]->state == TASK_ZOMBIE) {
        free_page((unsigned long)task[i]);
        task[i] = 0;
        task_used[i] = 0;
        continue;
      }
      if (current->pid != i) {
        if (task[i]->state == TASK_RUNNING) {
          last_sched_i = i;
          if (0 == i) {
            set_tss0_esp0(
                krn_stk0);  // 0任务的内核栈是写死的，不能用task[i])+PAGE_SIZE来计算
          } else {
            set_tss0_esp0((unsigned long)(task[i]) +
                          PAGE_SIZE);  //给切换栈机制搭桥
          }
          unsigned long esp0 = 0;
          get_esp0_when_switch(&esp0);
          current->kernel_stack = esp0;
          current = task[i];
          switch_to(_LDT(i), task[i]);
          return;
        }
      }
    }
  }
}

void do_timer(unsigned long cs) {
  if (0x8 == cs) {
  } else if (0xf == cs) {
    schedule();
  } else {
  }
}