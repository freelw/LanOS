#include "lanstd.h"

void lan_main() {
  char buffer[124];
  s_ls(buffer);
  int cnt = *((int *)buffer);
  s_print_str("There are ");
  s_print_num(cnt);
  s_print_str(" files in our filesystem:");
  s_print_crlf();
  s_print_str("---------------");
  s_print_crlf();
  for (int i = 0; i < cnt; ++i) {
    char *start = buffer + 4 + i * 12;
    int length = *((int *)start);
    char name[9] = {0};
    for (int j = 0; j < 8; ++j) {
      name[j] = (start + 4)[j];
    }
    s_print_str(name);
    s_print_str(" ");
    s_print_num(length);
    s_print_str(" bytes");
    s_print_crlf();
  }
  s_print_str("---------------");
  s_print_crlf();
  s_exit();
}
