#ifndef LANSTD_H
#define LANSTD_H
/*
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
*/
int s_test_sys_call();
int s_fork();
int s_test_sys_call2();
int s_print_str(char*);
int s_print_num(int);
int s_read_file_content(char*, char *);
int s_exec(char*);
int s_get_keyboad_buffer(char*);
int s_clean_keyboard();
int s_print_shell(char*);
int s_exit();
#endif