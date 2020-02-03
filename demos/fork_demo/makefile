all: link
	dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc
	dd if=lan_os of=a.img bs=512 count=17 skip=8 seek=1 conv=notrunc
	head -c 1474560 /dev/zero > a.vfd
	dd if=a.img of=a.vfd bs=512 count=18 conv=notrunc
link: head lan_main boot
	ld -m elf_i386 -T n.lds -o lan_os head.o lan_main.o sys_call.o intel_err_handle.o sched.o fork.o mm.o
head:
	nasm -felf32 -o head.o head.s 
lan_main:
	gcc -m32  -fno-stack-protector -fno-pie -c lan_main.c sys_call.c intel_err_handle.c sched.c fork.c mm.c
boot:
	nasm -o boot.bin boot.s -l boot.lst
clean:
	rm lan_os lan_main.o sys_call.o intel_err_handle.o head.o sched.o fork.o mm.o boot.bin boot.lst a.img a.vfd

