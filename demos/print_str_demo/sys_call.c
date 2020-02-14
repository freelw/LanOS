typedef int (*sys_call)();
extern void print_str(char *s);

int _test_sys_call() {
    print_str("ggg");
    return 0;
}

int _test_sys_call1() {
    return 0;
}

int _test_sys_call2() {
    print_str("hhh");
    return 0;  
}

extern void sys_fork();

extern sys_call sys_call_table[] = {
    _test_sys_call, sys_fork, _test_sys_call1, _test_sys_call2
};