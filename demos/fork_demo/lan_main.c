extern void write_char(char ch);
extern void open_a20();
extern void init_8259A();
extern void timer_interrupt();
extern void init_latch();
extern void assign_cr3_cr0(unsigned long);
extern void page_fault();
extern void system_call();
extern void _e0();
extern void _e1();
extern void _e2();
extern void _e3();
extern void _e4();
extern void _e5();
extern void _e6();
extern void _e7();
extern void _e8();
extern void _e9();
extern void _e10();
extern void _e11();
extern void _e12();
extern void _e13();
extern void _e14();
extern void _e15();
extern void _e16();

#define PAGE_DIR 0x8000

#include "gate_tool.h"
#include "move_to_user_mode.h"

#define __NR_test_sys_call 0
_syscall0(void, test_sys_call)

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
	unsigned long *pg_dir = PAGE_DIR; //lan_os最多不能超过8k
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
	write_char('L');
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
	init_8259A();
	set_intr_gate(0x20, &timer_interrupt);
	//set_trap_gate(14, &page_fault);
	set_system_gate(0x80, &system_call);

	set_trap_gate(0,&_e0);
	set_trap_gate(1,&_e1);
	set_trap_gate(2,&_e2);
	set_system_gate(3,&_e3);	/* int3-5 can be called from all */
	set_system_gate(4,&_e4);
	set_system_gate(5,&_e5);
	set_trap_gate(6,&_e6);
	set_trap_gate(7,&_e7);
	set_trap_gate(8,&_e8);
	set_trap_gate(9,&_e9);
	set_trap_gate(10,&_e10);
	set_trap_gate(11,&_e11);
	set_trap_gate(12,&_e12);
	set_trap_gate(13,&_e13);
	set_trap_gate(14,&_e14);
	set_trap_gate(15,&_e15);
	set_trap_gate(16,&_e16);

	sti();
	move_to_user_mode();
	while(1) {
		//for (int i = 0; i < 1000000; ++ i);
		test_sys_call();		
	}
}

void do_timer()
{
	write_char('T');
}
