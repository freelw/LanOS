all: link
	dd if=test_elf of=loop bs=512 skip=8 conv=notrunc
link: lan_main 
	ld -m elf_i386 -T n.lds -o test_elf lan_main.o
lan_main:
	gcc -m32 -g -fno-stack-protector -fno-pie -c lan_main.c 
clean:
	rm loop test_elf lan_main.o
cp:
	cp loop ../../root_fs/
	