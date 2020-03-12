#include "lanstd.h"

void lan_main()
{
	s_print_str("I am a test executable file.");
	s_print_str("I can print a number.");
	s_print_num(222);
	char buffer[64] = {0};
	s_read_file_content("hello_fs", buffer);
	s_print_str("hello_fs content is:");
	s_print_str(buffer);
	s_exit();
}