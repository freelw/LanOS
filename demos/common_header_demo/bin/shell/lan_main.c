#include "lanstd.h"

int equal_buffer(char *a, char *b, int len);
void lan_main() {
  s_print_str("--------------------------");
  s_print_str("|                        |");
  s_print_str("| welcome to lan_os 0.1! |");
  s_print_str("|                        |");
  s_print_str("--------------------------");
  s_print_str("shell started.");
  char buffer0[256];
  char buffer1[256];
  for (int i = 0; i < 256; ++i) {
    buffer0[i] = buffer1[i] = 0;
  }
  s_print_shell(buffer0);
  while (1) {
    s_get_keyboad_buffer(buffer0);
    if (0 != equal_buffer(buffer0, buffer1, 256)) {
      int enter = 0;
      for (int i = 0; i < 256; ++i) {
        if (buffer0[i] == 'E') {
          enter = 1;
          buffer0[i] = 0;
          break;
        }
      }
      if (enter) {
        s_clean_keyboard();
        if (!s_fork()) {
          s_exec(buffer0);
          s_exit();
        }
      }
      s_print_shell(buffer0);
    }
    for (int i = 0; i < 256; ++i) {
      buffer1[i] = buffer0[i];
    }
  }
}

int equal_buffer(char *a, char *b, int len) {
  for (int i = 0; i < len; ++i) {
    if (a[i] != b[i]) {
      return 1;
    }
  }
  return 0;
}