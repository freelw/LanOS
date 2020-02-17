typedef int (*sys_call)();
extern void print_str(char *s);

unsigned char get_fs_byte(const char * addr)
{
	unsigned register char _v;
	__asm__ ("movb %%fs:%1,%0":"=r" (_v):"m" (*addr));
	return _v;
}

extern inline void put_fs_byte(char val,char *addr)
{
__asm__ ("movb %0,%%fs:%1"::"r" (val),"m" (*addr));
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

int equal_str(char *file_name, char *name)
{
    for (int i = 0; i < 8; ++ i) {
        if (file_name[i] != name[i]) {
            return 1;
        } else if (file_name[i] == 0 && name[i] == 0) {
            return 0;
        }
    }
    return 0;
}

int _sys_read_file_content(char *_u_file_name, char *_u_buffer)
{
    char file_name[64];
    for (int i = 0; i < 64; ++ i) {
        file_name[i] = 0;
    }
    for (int i = 0; i < 8; ++ i) {
        file_name[i] = get_fs_byte(_u_file_name+i);
    }
    print_str("input file_name is:");
    print_str(file_name);
    int *p_cnt = 0x6dd84;
    char *meta_start = 0x6dd84+4;
    int file_cnt = *p_cnt;
    char *file_buffer_start = 0x6de00;
    for (int i = 0; i < file_cnt; ++ i) {
        char *start = file_buffer_start+20*1024*i;
        int *p_length = meta_start+12*i;
        char *name = meta_start+12*i+4;
        if (0 == equal_str(file_name, name)) {
            print_str("equal succ");
            for (int j = 0; j < *p_length; ++ j) {
                put_fs_byte(start[j], _u_buffer+j);
            }
        }
    }
    return 0;
}

int _sys_print_num(int num)
{
    char buf[64];
    int index = 0;
    for (int i = 0; i < 64; ++ i) {
        buf[i] = 0;
    }
    if (num == 0) {
        buf[index++] = '0';
    } else if (num < 0) {
        num = -num;
        buf[index++] = '-';
    }
    char tmp[64];
    int index_tmp = 0;
    while (num > 0) {
        tmp[index_tmp ++] = num%10 + '0';
        num /= 10;
    }
    for (int i = index_tmp-1; i >= 0; -- i) {
        buf[index++] = tmp[i];
    }
    print_str(buf);
    return 0;
}

int _sys_exec(char *_u_file_name)
{
    char file_name[64];
    for (int i = 0; i < 64; ++ i) {
        file_name[i] = 0;
    }
    for (int i = 0; i < 8; ++ i) {
        file_name[i] = get_fs_byte(_u_file_name+i);
    }
    return 0;
}

extern void sys_fork();
extern void sys_print_str();
extern void sys_print_num();
extern void sys_read_file_content();
extern void sys_exec();

extern sys_call sys_call_table[] = {
    _test_sys_call, sys_fork, _test_sys_call1, _test_sys_call2, sys_print_str, sys_print_num, sys_read_file_content, sys_exec
};