#include "gate_tool.h"

extern void _keyboard_interrupt();
extern void print_str(char *s);
extern void print_num(int num);
extern void print_hex(int num);
extern void panic(char *msg);

#define outb(value, port) __asm__("outb %%al,%%dx" ::"a"(value), "d"(port))

#define inb(port)                                             \
  ({                                                          \
    unsigned char _v;                                         \
    __asm__ volatile("inb %%dx,%%al" : "=a"(_v) : "d"(port)); \
    _v;                                                       \
  })

#define outb_p(value, port) \
  __asm__(                  \
      "outb %%al,%%dx\n"    \
      "\tjmp 1f\n"          \
      "1:\tjmp 1f\n"        \
      "1:" ::"a"(value),    \
      "d"(port))

#define inb_p(port)       \
  ({                      \
    unsigned char _v;     \
    __asm__ volatile(     \
        "inb %%dx,%%al\n" \
        "\tjmp 1f\n"      \
        "1:\tjmp 1f\n"    \
        "1:"              \
        : "=a"(_v)        \
        : "d"(port));     \
    _v;                   \
  })

struct pair {
  unsigned char code;
  char ch;
};

#define CODE_MAP_LEN 40
#define CODES_LEN 256

int input_code_index;
char codes[CODES_LEN];
struct pair code_map[CODE_MAP_LEN] = {
    {0x2, '1'},  {0x3, '2'},  {0x4, '3'},  {0x5, '4'},  {0x6, '5'},
    {0x7, '6'},  {0x8, '7'},  {0x9, '8'},  {0xa, '9'},  {0xb, '0'},
    {0x10, 'q'}, {0x11, 'w'}, {0x12, 'e'}, {0x13, 'r'}, {0x14, 't'},
    {0x15, 'y'}, {0x16, 'u'}, {0x17, 'i'}, {0x18, 'o'}, {0x19, 'p'},
    {0x1e, 'a'}, {0x1f, 's'}, {0x20, 'd'}, {0x21, 'f'}, {0x22, 'g'},
    {0x23, 'h'}, {0x24, 'j'}, {0x25, 'k'}, {0x26, 'l'}, {0x2c, 'z'},
    {0x2d, 'x'}, {0x2e, 'c'}, {0x2f, 'v'}, {0x30, 'b'}, {0x31, 'n'},
    {0x32, 'm'}, {0x1c, 'E'},
};

void get_codes(char *buffer) {
  for (int i = 0; i < CODES_LEN; ++i) {
    buffer[i] = 0;
  }
  for (int i = 0; i < input_code_index; ++i) {
    buffer[i] = codes[i];
  }
}

void parse_code(unsigned char code) {
  if (code == 0x0e) {
    if (input_code_index > 0) {
      codes[input_code_index--] = 0;
    }
    return;
  }
  for (int i = 0; i < CODE_MAP_LEN; ++i) {
    if (code == code_map[i].code) {
      codes[input_code_index++] = code_map[i].ch;
    }
  }
}

void keyboard_interrupt() {
  unsigned char code = inb_p(0x60);
  parse_code(code);
  outb_p(0x20, 0x20);
}

void clean_keyboard_buffer() {
  input_code_index = 0;
  for (int i = 0; i < CODES_LEN; ++i) {
    codes[i] = 0;
  }
}

void init_keyboard() {
  input_code_index = 0;
  for (int i = 0; i < CODES_LEN; ++i) {
    codes[i] = 0;
  }
  set_trap_gate(0x21, &_keyboard_interrupt);
  outb_p(inb_p(0x21) & 0xfd, 0x21);
  unsigned char a = inb_p(0x61);
  outb_p(a | 0x80, 0x61);
  outb_p(a, 0x61);
}