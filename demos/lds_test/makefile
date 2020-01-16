all:
	nasm -felf64 -o asm_int_80.o asm_int_80.s 
	ld -T n.lds -o asm_int_80 asm_int_80.o
clean:
	rm asm_int_80 asm_int_80.o

