extern void write_char(char ch);
extern void open_a20();
extern void init_8259A();
extern void timer_interrupt();
extern void init_latch();
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

#include "mm.h"
#include "sched.h"
#include "gate_tool.h"
#include "move_to_user_mode.h"

#define __NR_test_sys_call 0
#define __NR_fork 1
_syscall0(int, test_sys_call)
_syscall0(int, fork)

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
	mm_init();
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
	sched_init();
	sti();
	move_to_user_mode();
	if (fork()) {	
		while(1) {
			test_sys_call();		
		}
	} else {
		while (1) {
			/* code */
		}
	}
}

void do_timer()
{
	write_char('T');
}
