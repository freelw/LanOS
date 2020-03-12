#define _syscall0(type, name)                                       \
  type name(void) {                                                 \
    long __res;                                                     \
    __asm__ volatile("int $0x80" : "=a"(__res) : "0"(__NR_##name)); \
    if (__res >= 0) return (type)__res;                             \
    return -1;                                                      \
  }

#define _syscall1(type, name, atype, a)                   \
  type name(atype a) {                                    \
    long __res;                                           \
    __asm__ volatile("int $0x80"                          \
                     : "=a"(__res)                        \
                     : "0"(__NR_##name), "b"((long)(a))); \
    if (__res >= 0) return (type)__res;                   \
    return -1;                                            \
  }

#define _syscall2(type, name, atype, a, btype, b)                         \
  type name(atype a, btype b) {                                           \
    long __res;                                                           \
    __asm__ volatile("int $0x80"                                          \
                     : "=a"(__res)                                        \
                     : "0"(__NR_##name), "b"((long)(a)), "c"((long)(b))); \
    if (__res >= 0) return (type)__res;                                   \
    return -1;                                                            \
  }

#define _syscall3(type, name, atype, a, btype, b, ctype, c)              \
  type name(atype a, btype b, ctype c) {                                 \
    long __res;                                                          \
    __asm__ volatile("int $0x80"                                         \
                     : "=a"(__res)                                       \
                     : "0"(__NR_##name), "b"((long)(a)), "c"((long)(b)), \
                       "d"((long)(c)));                                  \
    if (__res >= 0) return (type)__res;                                  \
    return -1;                                                           \
  }

int s_print_str(char *);
int s_print_num(int);
int s_get_keyboad_buffer(char *);
int equal_buffer(char *a, char *b, int len);
int s_clean_keyboard();
int s_exec(char *);
int s_print_shell(char *);
int fork1();
int s_ls(char *);
int s_exit();

void lan_main() {
  s_print_str("ls result:");
  s_print_str("---------------");
  char buffer[124];
  s_ls(buffer);
  int cnt = *((int *)buffer);
  s_print_str("tot file cnt:");
  s_print_num(cnt);
  for (int i = 0; i < cnt; ++i) {
    char *start = buffer + 4 + i * 12;
    int length = *((int *)start);
    char name[9] = {0};
    for (int j = 0; j < 8; ++j) {
      name[j] = (start + 4)[j];
    }
    s_print_str(name);
    s_print_num(length);
  }
  s_print_str("---------------");
  s_exit();
}

#define __NR_test_sys_call 0
#define __NR_fork1 1
#define __NR_test_sys_call1 2
#define __NR_test_sys_call2 3
#define __NR_s_print_str 4
#define __NR_s_print_num 5
#define __NR_s_read_file_content 6
#define __NR_s_exec 7
#define __NR_s_get_keyboad_buffer 8
#define __NR_s_clean_keyboard 9
#define __NR_s_print_shell 10
#define __NR_s_ls 11
#define __NR_s_exit 12
_syscall0(int, test_sys_call) _syscall0(int, fork1)
    _syscall0(int, test_sys_call2) _syscall1(int, s_print_str, char *, msg)
        _syscall1(int, s_print_num, int, num)
            _syscall2(int, s_read_file_content, char *, file_name, char *,
                      buffer) _syscall1(int, s_exec, char *, file_name)
                _syscall1(int, s_get_keyboad_buffer, char *, buffer)
                    _syscall0(int, s_clean_keyboard)
                        _syscall1(int, s_print_shell, char *, buffer)
                            _syscall1(int, s_ls, char *, buffer)
                                _syscall0(int, s_exit)
