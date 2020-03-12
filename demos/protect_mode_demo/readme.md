# 梳理linux0.12知识点系列
# 之
# 让汇编到C不再遥远

## 背景
	
	在读《Linux内核完全剖析》这本书的时候，你是否有过这样的困境
	前期的基础知识沉浸在大量的汇编代码中,从16位实模式到32位保护
	模式，有要经历各种其他知识的冲击，比如8259A编程，A20地址线
	页表，等等。这些都在干扰我们有一条清晰的路线去观察系统是如何
	从汇编真正跳转到C代码中，于是让初学者感觉自己离真正的内核代码
	还好遥远，产生了深深的无力感。
	
## 目的
	
	本文的目的就是解决这样的问题，笔者希望用几个简单的demo来帮助
	读者梳理出汇编代码到c代码跳转的脉络，这样对于继续阅读0.12内
	核代码是很有帮助的。

## 和0.12的使用工具的区别

* 编译工具
	* 使用nasm作为汇编器，这样就不需要使用as86和gas两种汇编器
	* 使用新版本的gcc，比如笔者使用的是gcc7.4，而不需要使用书中提供的使用linux0.12进行编译的环境
	* 使用ld
* 虚拟机
	* 使用virtualbox

## 环境
* 以下是笔者使用的环境
	* ubuntu 18.04 上安装nasm
	* virtualbox运行在OSX上，需要将linux上编译出来的镜像下载下来

## 要点
* 本文要介绍以下4个demo，循序渐进，逐步达成我们的目标--在裸机上运行c代码
	1. 使用汇编在linux下调用系统调用，打印字符串
	2. 使用汇编在linux下调用c代码，c代码再调用汇编函数，打印字符串
	3. 在linux下编译出无elf头的镜像文件，验证链接脚本的使用方法
	4. 改造《linux源码剖析》第四章的demo，去掉多任务逻辑，加入跳转到c代码逻辑

## 获取代码
	
	https://github.com/freelw/LanOS.git
	
	在这个项目的demos文件夹下可以获取所有的代码

## 正式开始
### 1. 简单汇编demo

代码在`asm_int_80`目录下
	
	global _start
	_start:
		mov dword edx, 13
		mov dword ecx, message
		mov dword ebx, 1
		mov dword eax, 4
		int 0x80
		mov dword ebx, 0
		mov dword eax, 1
		int 0x80
	message:
		db      "Hello, World", 10 
		
这段代码展示了在linux系统下使用汇编调用系统调用，把字符串打印出来，这个demo的目的主要是验证我们的工作环境，编译器和连接器的功能是否正常

以下是编译和运行结果
	
	wangli@wangli-LC1:~/project/LanOS/demos/asm_int_80$ make
	nasm -felf64 -o asm_int_80.o asm_int_80.s
	ld -o asm_int_80 asm_int_80.o
	wangli@wangli-LC1:~/project/LanOS/demos/asm_int_80$ ./asm_int_80
	Hello, World

### 2. 汇编调用C代码

代码在`asm_call_c`目录下
首先是asm_int_80.s文件

	global _start, write
	extern lan_main
	_start:
		mov dword edx, 13
		mov dword ecx, message
		mov dword ebx, 1
		mov dword eax, 4
		int 0x80
	    push dword lan_main
		ret
	message:
	        db      "Hello, World", 10 
	message_c_write:
	        db      "Hello, Write", 10 
	write:
		mov dword edx, 13
		mov dword ecx, message_c_write
		mov dword ebx, 1
		mov dword eax, 4
		int 0x80
		mov dword ebx, 0
		mov dword eax, 1
		int 0x80
		
这段代码向外暴露了两个符号 `_start, write` 其中`write`用来给c文件调用，功能是打印一个字符串
另外定义了一个外部符号`lan_main`

接下来是c代码lan_main.c

	extern void write();
	void lan_main()
	{
		write();
	}

这段代码实现`lan_main`函数，并定义了一个外部符号 `write`

然后看看makefile是怎么写的

	all:
		gcc -c lan_main.c
		nasm -felf64 -o asm_int_80.o asm_int_80.s 
		ld -o asm_int_80 lan_main.o asm_int_80.o
	clean:
		rm asm_int_80 asm_int_80.o lan_main.o
	
可以看到分别使用gcc和nasm编译出两个.o文件然后在用链接器ld把他们链接在一起，这时候汇编中就知道了lan_main函数的地址而c中也知道了write函数的地址

以下是执行结果
	
	wangli@wangli-LC1:~/project/LanOS/demos/asm_call_c$ make
	gcc -c lan_main.c
	nasm -felf64 -o asm_int_80.o asm_int_80.s
	ld -o asm_int_80 lan_main.o asm_int_80.o
	wangli@wangli-LC1:~/project/LanOS/demos/asm_call_c$ ./asm_int_80
	Hello, World
	Hello, Write

这时我们确实是通过汇编跳入了c程序，看样子和我们的目标“在裸机上运行c代码”比较接近了

但是这里还存在几个问题

1. 在屏幕上显示字符我们不能使用int 0x80提供的功能，因为我们自己并没有实现系统调用，我们的最后真正的做法是直接在ring 0的状态下写显存
2. 生成的二进制文件的排布是有问题的，我们希望这个生成的二进制文件的入口在0x0处，这样，当我们使用引导扇区将这个二进制放到0x0处并跳转到此处时，符号的地址都是正确的
	
所以这时让我们来看一下刚才编译出来的二进制文件的内存布局
		
	
	wangli@wangli-LC1:~/project/LanOS/demos/asm_call_c$ objdump -S asm_int_80
	asm_int_80:     file format elf64-x86-64
	Disassembly of section .text:
	00000000004000b0 <lan_main>:
	  省略	
	00000000004000d0 <_start>:
	  省略
	00000000004000ec <message>:
	  省略
	00000000004000f9 <message_c_write>:
	  省略	
	0000000000400106 <write>:
	  省略

这里我们实际上时想让_start在执行时和存储是都在0x0地址上，但是这里显示在执行时的地址是0x4000ec（注意这个符号现在存储的地址我们还不知道，但应该也不是0x0）

这个布局其实是在链接阶段确定的，如果我们能够在这时控制链接器的行为，我们就能够控制布局

### 使用链接脚本

我们可以使用一个lds文件来告诉ld我们的布局需求，lds被称作链接脚本，有兴趣请自行查询其规则，这里先简单列出我们使用的n.lds 

代码在`lds_test`目录下
	
	SECTIONS
	{
	. = 0x0;
	.text : { *(.text) }
	}

大概的意思是把代码段放在0x0处

这时我们再来看内存布局

	wangli@wangli-LC1:~/project/LanOS/demos/lds_test$ objdump -S asm_int_80
	asm_int_80:     file format elf64-x86-64
	Disassembly of section .text:
	0000000000000000 <_start>:	
	0:	ba 0d 00 00 00       	mov    $0xd,%edx
	0000000000000022 <message>:
	...

这时发现_start在加载后的位置正确了	
	
那这时我们要看一下存储的位置，也就是在文件中的位置，是不是也是在0x0处，这个对我们非常重要，我们必须知道_start存储的其实地址，或者把它强制放在文件的0地址处，我们才能写代码将它正确的加载到内存的0地址处

	od -h asm_int_80 | head
	0000000 457f 464c 0102 0001 0000 0000 0000 0000
	0000020 0002 003e 0001 0000 0000 0000 0000 0000
	0000040 0040 0000 0000 0000 00e8 0020 0000 0000
	0000060 0000 0000 0040 0038 0001 0040 0005 0004
	0000100 0001 0000 0005 0000 0000 0020 0000 0000
	0000120 0000 0000 0000 0000 0000 0000 0000 0000
	0000140 002f 0000 0000 0000 002f 0000 0000 0000
	0000160 0000 0020 0000 0000 0000 0000 0000 0000
	0000200 0000 0000 0000 0000 0000 0000 0000 0000

使用od命令看一下文件的头部的二进制，发现和_start位置的二进制完全不同，这是因为我们编译出来的文件是一个elf文件，头部有很多信息描述自己的结构，我们现在需要把头砍掉，直接找到.text段，强制放到0地址处

	wangli@wangli-LC1:~/project/LanOS/demos/lds_test$ readelf -l asm_int_80

	Elf file type is EXEC (Executable file)
	Entry point 0x0
	There is 1 program header, starting at offset 64
	
	Program Headers:
	  Type           Offset             VirtAddr           PhysAddr
	                 FileSiz            MemSiz              Flags  Align
	  LOAD           0x0000000000200000 0x0000000000000000 0x0000000000000000
	                 0x000000000000002f 0x000000000000002f  R E    0x200000
	
	 Section to Segment mapping:
	  Segment Sections...
	   00     .text
	
使用`readelf -l`命令查看.text段段偏移，发现偏移了0x200000,如果按照512为一个扇区的话，一共偏移了刚好4096个扇区，所以在下一个例子中会看到，makefile中最后会砍掉这个文件的前4096个扇区

### 最终demo
	

代码在`protect_mode_demo`目录中呢

这个demo重写了《linux内核完全剖析》第四章的汇编代码

第四章的汇编代码主要分为两个部分

1.引导扇区，负责从文件中把第一个扇区到第18个扇区到数据通过bios提供的读磁盘功能放到内存0x0处，并构造一个临时gdt，进入保护模式，跳转到 8:0处
2.第二个汇编代码的在磁盘上的起始地址是512,运行时在内存中的地址是0x0,这段代码的功能是再次初始化好gdt和idt，手动构建两个任务，iret到用户态，使用时中中断来切换两个任务

我们把第二个代码的多任务部分去掉，修改为ret到一个名为`lan_main`的c函数中，在c函数中调用一个汇编函数write_char，打印"LOVE",并最终在此死循环

以下是引导扇区代码loader.s

	[BITS 16]
	ORG 07c00h
	SYSSEG equ 01000h
	SYSLEN equ 17
	jmp 07c0h:(load_system-$)
	
	load_system:
	    mov dx, 00000h
	    mov cx, 00002h
	    mov ax, SYSSEG
	    mov es, ax              ;es:bx 01000h:0h bios读取磁盘写入内存的目标位置
	    xor bx, bx
	    mov ax, 0200h+SYSLEN    ;ah 读扇区功能号2 al读扇区数量 17
	    int 013h
	    jnc ok_load
	    jmp $
	
	ok_load:
	    cli
	    mov ax, SYSSEG          ;开始把010000h位置的数据拷贝到0h处
	    mov ds, ax              ;注意这时bios的代码就会被冲掉，无法再使用int 10h
	    xor ax, ax
	    mov es, ax
	    mov cx, 0x1000
	    sub si, si
	    sub di, di
	    cld                     ;df = 0 rep movsw是正向的
	    rep movsw
	    mov ax, 0x0           ;重新恢复ds指向0x0
	    mov ds, ax
	    lgdt [gdt_48]           ;ds+gdt_48 因为第一句话ORG 07c00h 所以此时gdt_48这个常量是：07c00h+到文件首的偏移
	    mov ax, 0x0001
	    lmsw ax
	    jmp dword 8:0
	gdt:
	    dw 0, 0, 0, 0           ;第一个描述符，没有用
	    dw 0x07ff               ;代码段 从0地址开始
	    dw 0x0000
	    dw 0x9a00
	    dw 0x00c0
	    dw 0x07ff               ;数据段 从0地址开始
	    dw 0x0000
	    dw 0x9200
	    dw 0x00c0
	
	
	gdt_48:
	    dw 0x7ff                ;2048/8=256个描述符
	    dw gdt, 0        ;基地址是从0x7c00开始的gdt位置
	
	;----------注意！所有的有效语句要写在这之前，并且总长出小于等于510字节----------
	    times 510 - ($-$$) db 0
	    dw 0xaa55

上面的代码会将下面两个文件链接的结果加载到0x0地址处，并跳转到这里以保护模式开始执行

我们看一下boot.s
	
	[BITS 32]
	
	LATCH equ 11930
	SCRN_SEL equ 0x18
	TSS0_SEL equ 0x20
	LDT0_SEL equ 0x28
	TSS1_SEL equ 0x30
	LDT1_SEL equ 0x38
	global write_char
	extern lan_main
	start_up32:
	    mov dword eax, 0x10 ;这时候使用的0x10还是loader.asm中定义的,虽然boot.asm之后定义的0x10描述符与之完全相同
	    mov ds, ax
	    lss esp, [init_stack];接下来要使用call指令，所以这里要初始化好栈
	    call setup_gdt
	    call setup_idt
	
	    mov eax, 0x10   ;加载完gdt之后重新加载所有的段寄存器，因为要更新段寄存器中段描述符的缓存（不可见部分）参见《linux内核完全剖析》94页
	    mov ds, ax
	    mov es, ax
	    mov fs, ax
	    mov gs, ax
	
	    lss esp, [init_stack];因为ds可能更新了（这个例子中实际上没有），所以要重新加载ss
	    push dword lan_main
	    ret
	
	setup_gdt:
	    lgdt [lgdt_48]
	    ret
	
	setup_idt:
	    lea edx, [ignore_int]
	    mov eax, dword 0x00080000
	    mov ax, dx
	    mov dx, 0x8e00
	    lea edi, [idt]
	    mov ecx, 256
	rp_idt:
	    mov dword [edi], eax
	    mov dword [edi+4], edx
	    add dword edi, 8
	    dec ecx
	    jne rp_idt
	    lidt [lidt_48]
	    ret
	
	write_char:
	    push gs
	    push dword ebx
	    mov ebx, SCRN_SEL
	    mov gs, bx
	    mov bx, [src_loc]
	    shl ebx, 1
	    push dword eax
	    mov eax, edi
	    mov byte [gs:ebx], al
	    pop dword eax
	    shr ebx, 1
	    inc dword ebx
	    cmp dword ebx, 2000
	    jb not_equ          ;jb : jump if below
	    mov dword ebx, 0
	not_equ:
	    mov dword [src_loc], ebx
	    pop dword ebx
	    pop gs
	    ret
	
	align 4
	ignore_int:
	    iret
	
	current: dd 0
	src_loc: dd 0
	
	align 4
	lidt_48:
	    dw 256*8-1
	    dd idt
	lgdt_48:
	    dw end_gdt-gdt-1
	    dd gdt
	
	align 8
	idt:
	    times 256 dq 0
	gdt:
	    dq 0x0000000000000000
	    dq 0x00c09a00000007ff   ;0x08 这两个段描述符和loader.asm中的代码段数据段是一样的
	    dq 0x00c09200000007ff   ;0x10
	    dq 0x00c0920b80000002   ;0x18 显存数据段
	end_gdt:
	
	    times 128 dd 0
	init_stack:         ;从这里开始是一个48位操作数
	    dd init_stack   ;32位代表初始的esp
	    dw 0x10         ;16位栈的段选择符，lss之后会加载到ss中

最后看一下c代码lan_main.c

	extern void write_char(char ch);
	void lan_main()
	{
		write_char('L');
		write_char('O');
		write_char('V');
		write_char('E');
		while(1);
	}

这段代码甚至可以用c++重写，见`protect_mode_cpp_demo`

文件lan_main.cpp
	
	extern "C" void write_char(char ch);
	class L
	{
	public:
		L() {
		    write_char('L');
		}
	};
	class O: public L
	{
	public:
		O() {
		    write_char('O');
		}
	};
	
	class V: public O
	{
	public:
		V() {
		    write_char('V');
		}
	};
	
	class E: public V
	{
	public:
		E() {
		    write_char('E');
		}
	};
	
	extern "C" void lan_main()
	{
		E e;
		while(1);
	}

编译的结果

	wangli@wangli-LC1:~/project/LanOS/demos/protect_mode_cpp_demo$ make
	nasm -felf64 -o boot.o boot.s
	gcc -c lan_main.cpp
	nasm -o loader.bin loader.s -l loader.lst
	ld -T n.lds -o lan_os boot.o lan_main.o
	dd if=loader.bin of=a.img bs=512 count=1 conv=notrunc
	1+0 records in
	1+0 records out
	512 bytes copied, 0.000231116 s, 2.2 MB/s
	dd if=lan_os of=a.img bs=512 count=17 skip=4096 seek=1 conv=notrunc
	10+1 records in
	10+1 records out
	5248 bytes (5.2 kB, 5.1 KiB) copied, 0.000285108 s, 18.4 MB/s
	head -c 1474560 /dev/zero > a.vfd
	dd if=a.img of=a.vfd bs=512 count=18 conv=notrunc
	11+1 records in
	11+1 records out
	5760 bytes (5.8 kB, 5.6 KiB) copied, 0.000273321 s, 21.1 MB/s

执行结果

![](https://raw.githubusercontent.com/freelw/LanOS/master/demos/pic/cpp.png)

有意思吧
	
## 总结

	是不是觉得很快就从汇编转入了c的世界呢，这样是不是觉得对内核代码的把控更有信心了呢，“原来这
	样就能用c开始编写内核代码了啊！”，如果你有这样的感慨，并且会有稍稍不再畏惧翻阅0.12代码的心
	情，那么本文的目的就达到了。









	

