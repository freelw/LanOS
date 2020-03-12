extern void write_char(char ch);
extern void open_a20();
extern void init_8259A();
extern void timer_interrupt();
extern void init_latch();
extern void set_sti();

#include "gate_tool.h"

void check_a20_valid() {
  unsigned char *p0 = 0x0;
  unsigned char *p1 = 0x100000;
  for (unsigned char i = 0; i < 10; ++i) {
    *p0 = i;
    if (*p0 != *p1) {
      write_char('b');
      break;
    } else {
      write_char('e');
    }
  }
}

void lan_main() {
  write_char('L');
  write_char('O');
  write_char('V');
  write_char('E');
  open_a20();
  check_a20_valid();
  init_latch();
  init_8259A();
  set_intr_gate(0x20, &timer_interrupt);
  set_sti();
  while (1)
    ;
}

void do_timer() {
  write_char('T');
  write_char('I');
  write_char('M');
  write_char('E');
  write_char('R');
}
