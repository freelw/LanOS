#include "lanstd.h"

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
