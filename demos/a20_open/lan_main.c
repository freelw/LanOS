extern void write_char(char ch);
extern void open_a20();

void check_a20_valid()
{
    unsigned char *p0 = 0x0;
    unsigned char *p1 = 0x100000;
	for (unsigned char i = 0; i < 10; ++ i) {
		*p0 = i;   
		if (*p0 != *p1) {
			write_char('b');
			break;
		} else {
			write_char('e');
		}
	}
}

void lan_main()
{
	write_char('L');
	write_char('O');
	write_char('V');
	write_char('E');
	open_a20();
	check_a20_valid();
	while(1);
}
