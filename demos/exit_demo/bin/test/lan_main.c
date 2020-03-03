#define _syscall1(type,name,atype,a) \
type name(atype a) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(a))); \
if (__res >= 0) \
	return (type) __res; \
return -1; \
}

int s_print_str(char*);
int s_print_num(int);

void lan_main()
{
    s_print_str("I am a test executable file !");
    s_print_str("I can print a number.");
    s_print_num(222);
    while(1);
}
#define __NR_s_print_str 4
#define __NR_s_print_num 5
_syscall1(int, s_print_str, char*, msg)
_syscall1(int, s_print_num, int, num)