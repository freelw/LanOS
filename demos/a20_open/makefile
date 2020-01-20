all: link
	dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc
	dd if=lan_os of=a.img bs=512 count=17 skip=4096 seek=1 conv=notrunc
	head -c 1474560 /dev/zero > a.vfd
	dd if=a.img of=a.vfd bs=512 count=18 conv=notrunc
link: head lan_main boot
	ld -T n.lds -o lan_os head.o lan_main.o
head:
	nasm -felf64 -o head.o head.s 
lan_main:
	gcc -c lan_main.c
boot:
	nasm -o boot.bin boot.s -l boot.lst
clean:
	rm lan_os lan_main.o head.o boot.bin boot.lst a.img a.vfd

