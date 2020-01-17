all: link
	dd if=loader.bin of=a.img bs=512 count=1 conv=notrunc
	dd if=lan_os of=a.img bs=512 count=17 skip=4096 seek=1 conv=notrunc
	head -c 1474560 /dev/zero > a.vfd
	dd if=a.img of=a.vfd bs=512 count=18 conv=notrunc
link: boot lan_main loader
	ld -T n.lds -o lan_os boot.o lan_main.o
boot:
	nasm -felf64 -o boot.o boot.s 
lan_main:
	gcc -c lan_main.cpp
loader:
	nasm -o loader.bin loader.s -l loader.lst
clean:
	rm lan_os lan_main.o boot.o loader.bin loader.lst a.img a.vfd

