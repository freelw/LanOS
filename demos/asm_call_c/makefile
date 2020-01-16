all:
	gcc -c lan_main.c
	nasm -felf64 -o asm_int_80.o asm_int_80.s 
	ld -o asm_int_80 lan_main.o asm_int_80.o
clean:
	rm asm_int_80 asm_int_80.o lan_main.o

