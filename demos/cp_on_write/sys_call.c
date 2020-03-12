extern void write_char(char ch);
typedef int (*sys_call)();

int _test_sys_call() {
  write_char('f');
  return 0;
}

int _test_sys_call1() {
  write_char('g');
  return 0;
}

int _test_sys_call2() {
  write_char('h');
  return 0;
}

extern void sys_fork();

extern sys_call sys_call_table[] = {_test_sys_call, sys_fork, _test_sys_call1,
                                    _test_sys_call2};