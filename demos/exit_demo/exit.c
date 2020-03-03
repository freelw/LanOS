#include "mm.h"
#include "sched.h"
void _sys_exit()
{
    current->state = TASK_ZOMBIE;
    unsigned long data_base = get_data_base(current);
    unsigned long data_limit = get_limit(0x17);
    free_page_tables(data_base, data_limit);
    schedule();
}