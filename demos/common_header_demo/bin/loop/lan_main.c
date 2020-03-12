#include "lanstd.h"

void lan_main()
{
	int i = 0;
    while (1) {
		s_print_str("im a loop.");
		s_print_num(i++);
		for (int j = 0; j < 10000000; ++ j);
	}
}