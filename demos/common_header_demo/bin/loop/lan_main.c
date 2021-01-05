#include "lanstd.h"

void lan_main() {
  s_print_str("start printing 20 times.");
  s_print_crlf();
  for (int i = 0; i < 200; ++ i) {
    s_print_str("im a loop.");
    s_print_num(i);
    s_print_crlf();
    for (int j = 0; j < 10000000; ++j)
      ;
  }
  s_exit();
}