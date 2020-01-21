# 梳理linux0.12知识点系列 
# 之
# 8259A的初始化和时钟中断

## 背景

	cpu和设备协同工作的高效方式是使用中断机制，本例子机遇之前的打
	开了A20地址线之后的demo，进一步初始化了8259A终端控制芯片，并
	且屏蔽了除了始终中断以外的所有硬件中断。当始终中断发生时，在
	屏幕上打印'T' 'I' 'M' 'E' 'R'五个字符

## 额外修改

	除了加入初始化8259A芯片和增加中断处理函数的逻辑之外，本例子
	将gcc和ld的处理方式修改为32位模式，并且makefile中去掉二进制	头的操作修改了偏移量。这是因为我们会从linux0.12的system.h代
	码中扣出几个嵌入汇编的宏来使用，这几个嵌入汇编如果默认用64位的
	模式编译的话生成的二进制是错误的

### 修改细节
	
* makefile中
	
`dd if=lan_os of=a.img bs=512 count=17 skip=8 seek=1 conv=notrunc`
skip=4096变成了skip=8

ld 加入了 `-m elf_i386` 选项

gcc 加入了 `-m32` 选项

* head.s中
	
write_char取传入参数不再从edi寄存器去，而是从`[esp+16]`这个地址取

`mov eax, [esp+16]`

## cpu于8259A的连接方式

以下内容摘自《ORANGE'S:一个操作系统的实现》
![](https://raw.githubusercontent.com/freelw/LanOS/master/demos/pic/orange1.png)

    