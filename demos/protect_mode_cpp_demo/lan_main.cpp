extern "C" void write_char(char ch);
class L {
 public:
  L() { write_char('L'); }
};
class O : public L {
 public:
  O() { write_char('O'); }
};

class V : public O {
 public:
  V() { write_char('V'); }
};

class E : public V {
 public:
  E() { write_char('E'); }
};

extern "C" void lan_main() {
  E e;
  while (1)
    ;
}