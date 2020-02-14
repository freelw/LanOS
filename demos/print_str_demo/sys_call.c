typedef int (*sys_call)();
extern void print_str(char *s);

unsigned char get_fs_byte(const char * addr)
{
	unsigned register char _v;
	__asm__ ("movb %%fs:%1,%0":"=r" (_v):"m" (*addr));
	return _v;
}

int _test_sys_call()
{
    print_str("ggg");
    return 0;
}

int _test_sys_call1()
{
    return 0;
}

int _test_sys_call2()
{
    print_str("hhh");
    return 0;  
}

int _sys_print_str(char *msg)
{
    char buf[64];
    for (int j = 0; j < 64; ++ j) {
        buf[j] = 0;
    }
    int i = 0;
    while (1) {
        buf[i] = get_fs_byte(msg+i);
        if (buf[i] == 0) {
            break;
        }
        i++;
    }
    print_str(buf);
    return 0;
}

extern void sys_fork();
extern void sys_print_str();

extern sys_call sys_call_table[] = {
    _test_sys_call, sys_fork, _test_sys_call1, _test_sys_call2, sys_print_str
};