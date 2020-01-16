all: boot lan_main loader
	ld -T n.lds -o lan_os boot.o lan_main.o
boot:
	nasm -felf64 -o boot.o boot.s 
lan_main:
	gcc -c lan_main.c
loader:
	nasm -o loader.bin loader.asm -l loader.lst
clean:
	rm lan_main lan_main.o boot.o loader.bin loader.lst

