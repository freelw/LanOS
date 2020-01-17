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

代码在asm_int_80目录下
	
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

代码在asm_call_c目录下
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

这时候我们需要在链接器链接




	

