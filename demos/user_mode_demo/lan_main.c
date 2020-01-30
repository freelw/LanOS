extern void write_char(char ch);
extern void open_a20();
extern void init_8259A();
extern void timer_interrupt();
extern void init_latch();
extern void assign_cr3_cr0(unsigned long);
extern void page_fault();

#include "gate_tool.h"
#include "move_to_user_mode.h"

void check_a20_valid()
{
    unsigned char *p0 = 0x0;
    unsigned char *p1 = 0x100000;
	for (unsigned char i = 0; i < 10; ++ i) {
		*p0 = i;   
		if (*p0 != *p1) {
			write_char('b');
			break;
		} else {
			write_char('e');
		}
	}
}

void setup_paging()
{
	unsigned long *pg_dir = 0x8000; //lan_os最多不能超过8k
	unsigned long *pg0 = (unsigned long)(pg_dir) + 0x1000;
	unsigned long *pg1 = (unsigned long)(pg0) + 0x1000;
	unsigned long *pg2 = (unsigned long)(pg1) + 0x1000;
	unsigned long *pg3 = (unsigned long)(pg2) + 0x1000;
	pg_dir[0] = (unsigned long)(pg0) + 7;
	pg_dir[1] = (unsigned long)(pg1) + 7;
	pg_dir[2] = (unsigned long)(pg2) + 7;
	pg_dir[3] = (unsigned long)(pg3) + 7;
	for (int i = 0; i < 0x1000; ++ i) {
		pg0[i] = (i << 12) + 7;
	}
	assign_cr3_cr0(pg_dir);
}

void lan_main()
{
	init_8259A();
	/*write_char('L');
	write_char('O');
	write_char('V');
	write_char('E');
	write_char('U');
	write_char('S');
	write_char('E');
	write_char('R');
	open_a20();
	check_a20_valid();
	setup_paging();
	init_latch();
	
	set_intr_gate(0x20, &timer_interrupt);
	set_trap_gate(14, &page_fault);*/
	sti();
	move_to_user_mode();
	while(1);
}

void do_timer()
{
	write_char('T');
}
