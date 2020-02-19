#include "gate_tool.h"

extern void _keyboard_interrupt();

#define outb_p(value,port) \
__asm__ ("outb %%al,%%dx\n" \
		"\tjmp 1f\n" \
		"1:\tjmp 1f\n" \
		"1:"::"a" (value),"d" (port))

#define inb_p(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al\n" \
	"\tjmp 1f\n" \
	"1:\tjmp 1f\n" \
	"1:":"=a" (_v):"d" (port)); \
_v; \
})

void keyboard_interrupt()
{

}

void init_keyboard()
{
    set_trap_gate(0x21,&_keyboard_interrupt);
	outb_p(inb_p(0x21)&0xfd,0x21);
	unsigned char a=inb_p(0x61);
	outb_p(a|0x80,0x61);
	outb_p(a,0x61);
}