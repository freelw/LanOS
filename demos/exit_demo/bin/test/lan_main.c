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
int s_exit();

int s_print_str(char*);
int s_read_file_content(char*, char*);
int s_print_num(int);

void lan_main() {
  s_print_str("I am a test executable file !");
  s_print_str("I can print a number.");
  s_print_num(222);
  char buffer[64] = {0};
  s_read_file_content("hello_fs", buffer);
  s_print_str("hello_fs content is:");
  s_print_str(buffer);
  s_exit();
}
#define __NR_s_print_str 4
#define __NR_s_print_num 5
#define __NR_s_exit 12
#define __NR_s_read_file_content 6
_syscall2(int, s_read_file_content, char*, file_name, char*, buffer)
    _syscall1(int, s_print_str, char*, msg)
        _syscall1(int, s_print_num, int, num) _syscall0(int, s_exit)