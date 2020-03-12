#define _syscall0(type,name) \
type name(void) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name)); \
if (__res >= 0) \
	return (type) __res; \
return -1; \
}

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

#define _syscall2(type,name,atype,a,btype,b) \
type name(atype a,btype b) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(a)),"c" ((long)(b))); \
if (__res >= 0) \
	return (type) __res; \
return -1; \
}

#define _syscall3(type,name,atype,a,btype,b,ctype,c) \
type name(atype a,btype b,ctype c) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(a)),"c" ((long)(b)),"d" ((long)(c))); \
if (__res>=0) \
	return (type) __res; \
return -1; \
}

#define __NR_s_test_sys_call 0
#define __NR_s_fork 1
#define __NR_s_test_sys_call1 2
#define __NR_s_test_sys_call2 3
#define __NR_s_print_str 4
#define __NR_s_print_num 5
#define __NR_s_read_file_content 6
#define __NR_s_exec 7
#define __NR_s_get_keyboad_buffer 8
#define __NR_s_clean_keyboard 9
#define __NR_s_print_shell 10
#define __NR_s_exit 12
_syscall0(int, s_test_sys_call)
_syscall0(int, s_fork)
_syscall0(int, s_test_sys_call2)
_syscall1(int, s_print_str, char*, msg)
_syscall1(int, s_print_num, int, num)
_syscall2(int, s_read_file_content, char*, file_name, char*, buffer)
_syscall1(int, s_exec, char*, file_name)
_syscall1(int, s_get_keyboad_buffer, char*, buffer)
_syscall0(int, s_clean_keyboard)
_syscall1(int, s_print_shell, char*, buffer)
_syscall0(int, s_exit)