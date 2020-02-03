extern void write_char(char ch);
typedef int (*sys_call)();

int _test_sys_call() {
    write_char('f');
    return 0;
}

extern void sys_fork();

extern sys_call sys_call_table[] = {
    _test_sys_call, sys_fork
};