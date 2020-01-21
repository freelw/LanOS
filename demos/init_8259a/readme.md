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
	将gcc和ld的处理方式修改为32位模式，并且makefile中去掉二进制
	头的操作修改了偏移量。这是因为我们会从linux0.12的system.h代
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

## cpu与8259A的连接方式

	(笔者解读)从下图可以看到主板上的物理连接关系，每片8259A有8根输入线，可以连接8个硬件设备
	
	拿时钟为例简单描述一下8259A和cpu常见的工作流程
	
	1.在机器上电初始化的时候我们对时钟芯片进行变成，命令它在震动一个固定次数后要给cpu发送信号
	2.cpu在运行的时候是不断的取指令，执行指令的
	3.当始终芯片到了固定的次数时，它不能直接给cpu的intr引脚发指令，为什么呢，因为如果这么设计
	  cpu就只能够接收时钟的消息而不能处理其他硬件的消息了，很显然我们要处理多种硬件才能使用计
	  算机
	4.这时候我们需要一个代理人帮我们接收各种硬件的消息，并且有一种机制通知cpu：1）有人发消息
	  给你了 2）这个人是某某某。 做这件事情的这个代理人就是8259A芯片
	5.一块8259A芯片可以连接8个设备，8259A在自己收到消息后通知cpu的intr引脚，cpu在执行完
	  当前的最后一条指令后去问8259A是谁发送的中断请求，这时8259A会把中断对应的中断号放在
	  数据总线上，cpu通过这个中断号查找自己的IDT表，找到中断处理函数并执行。
	6.这里我们能看到8259A的几个功能
		1）能够将自己的一个输入引脚对应到一个中断号
		2）必须考虑多个中断同时到达的情况，所以8259A可以对不同的中断有优先级处理和排队的能力
		3）8259A实际上还有能力屏蔽某个引脚的中断（这是显而易见的），在汇编代码中的实现方式就是
		   给8259A的一个io端口发送一个控制命令字，这个字节是一个掩码，只用对应位置是0，该中断
		   才能被转发给cpu的intr引脚
		4）如下面第一幅图所示，8259A是可以串联的，这样可以控制15个硬件
	 
	
		
以下内容摘自《ORANGE'S:一个操作系统的实现》
	
![](https://raw.githubusercontent.com/freelw/LanOS/master/demos/pic/orange1.png)

    中断产生的原因有两种，一种是外部中断，也就是由硬件产生的中断，
    另一种是由指令int n产生的中断。指令int n产生中断时的情形
    如图3.37所示，n即为向量号，它类似于调用门的使用。外部中断的情况
    则复杂一些，因为需要建立硬件中断与向量号之间的对应关系。外部中断
    分为不可屏蔽中断（NMI）和可屏蔽中断两种，分别由CPU的两根引脚NMI
    和INTR来接收，如图3.39所示。

![](https://raw.githubusercontent.com/freelw/LanOS/master/demos/pic/orange2.png)

	NMI不可屏蔽，因为它与IF是否被设置无关。NMI中断对应的中断向量号为2
	，这在表3.8中已经有所说明。可屏蔽中断与CPU的关系是通过对可编程中断
	控制器8259A建立起来的。如果你是第一次听说8259A，那么你可以认为它是
	中断机制中所有外围设备的一个代理，这个代理不但可以根据优先级在同时
	发生中断的设备中选择应该处

## 8259A的初始化

### 为什么要初始化：
1.Intel x86异常机制
	
	cpu除了能够从intr引脚和nmi引脚接收中断请求从而进入到相应的处理函数
	之外，还能够在自己发生特殊故障的时候以类似的方式进入故障处理逻辑，这就是
	异常机制，《ORANGE'S:一个操作系统的实现》书中的表3.8（上图）展示了各种
	可能出现的异常和对应的向量号
	异常处理函数和硬件中断处理函数需要排布在同一张表中，按照一个互相不冲突的
	顺序排列，这样cpu在收到中断或者是异常的时候会以类似的方式处理。
	
2.IBM 初始化了8259A

	IBM的bios在启动的时候初始化了`0x8-0xf`这几个号码作为8个硬件中断，这就
	与x86的内部异常号冲突了，所以我们如果想要正常使用cpu的功能的话，就必须
	对8259A重新初始化，做的事情很简单，就是把所有的硬件中断号制定到intel的
	保留中断号之后，也就是从32号开始。

### 如何初始化8259A：
以下内容摘自《ORANGE'S:一个操作系统的实现》

	还好，8259A是可编程中断控制器，对它的设置并不复杂，是通过向相应的端口写入
	特定的ICW（InitializationCommandWord）来实现的。主8259A对应的端口地址
	是20h和21h，从8259A对应的端口地址是A0h和A1h。ICW共有4个，每一个都是具
	有特定格式的字节。为了先对初始化8259A的过程有一个概括的了解，我们过一会儿
	再来关注每一个ICW的格式，现在，先来看一下初始化过程：
	1.往端口20h（主片）或A0h（从片）写入ICW1。
	2.往端口21h（主片）或A1h（从片）写入ICW2。
	3.往端口21h（主片）或A1h（从片）写入ICW3。
	4.往端口21h（主片）或A1h（从片）写入ICW4。
	这4步的顺序是不能颠倒的。我们现在来看一下4个如图3.40所示的ICW的格式。
![](https://raw.githubusercontent.com/freelw/LanOS/master/demos/pic/orange3.png)

### 抠代码的过程：
	
首先我们将书中的初始化8259A的部分扣出来，到我们的`head.s`中，在`init_8259a`目录下
	
	init_8259A:
	    mov al, 0x11
	    out 0x20, al
	    call io_delay
	    out 0xa0, al
	    call io_delay
	    mov al, 0x20
	    out 0x21, al
	    call io_delay
	    mov al, 0x28
	    out 0xa1, al
	    call io_delay
	    mov al, 0x4
	    out 0x21, al
	    call io_delay
	    mov al, 0x2
	    out 0xa1, al
	    call io_delay
	    mov al, 0x1
	    out 0x21, al
	    call io_delay
	    out 0xa1, al
	    call io_delay
	    mov al, 0xfe
	    out 0x21, al
	    call io_delay
	    mov al, 0xff
	    out 0xa1, al
	    call io_delay
	    ret
	
	io_delay:
	    nop
	    nop
	    nop
	    nop
	    ret
	    
然后把`init_8259A`这个函数暴露给c代码

	global write_char, open_a20, idt, init_latch, init_8259A, timer_interrupt, set_sti

这里面除了`init_8259A`相比于上一次还多暴露了

1. init_latch （从linux0.12代码中搬过来，初始化时钟芯片100ms发送一个中断请求）
2. timer_interrupt （时钟中断的处理函数地址）
3. set_sti （给c代码暴露一个函数用来执行sti指令）

我们看一下这三个函数的实现

	init_latch:
	    ;设置频率100Hz
	    mov byte al, 0x36
	    mov dword edx, 0x43
	    out dx, al
	    mov dword eax, LATCH
	    mov dword edx, 0x40
	    out dx, al
	    mov al, ah
	    out dx, al
	    
	timer_interrupt:
	    mov al, 0x20
	    out 0x20, al
	    call do_timer
	    iret

	set_sti:
		sti
		ret
	
其中timer_interrupt又调到了c代码中的`do_timer`函数去打印字符

在c代码`lan_main.c`中

	extern void write_char(char ch);
	extern void open_a20();
	extern void init_8259A();
	extern void timer_interrupt();
	extern void init_latch();
	extern void set_sti();

	#include "gate_tool.h"

	...

	void lan_main()
	{
		write_char('L');
		write_char('O');
		write_char('V');
		write_char('E');
		open_a20();
		check_a20_valid();
		init_latch();
		init_8259A();
		set_intr_gate(0x20, &timer_interrupt);
		set_sti();
		while(1);
	}
	
	...

上次a20地址线的demo中我们的lan_main函数只写到了`check_a20_valid()`就结束了，
这次我们继续往下，分别执行了

1. init_latch(); 初始化时钟
2. init_8259A(); 初始化8259A，这里按照《ORANGE'S:一个操作系统的实现》，把硬件中断放到了从0x20开始（linux也是从这里开始的）
3. set_intr_gate(0x20, &timer_interrupt); 把第一个硬件中断0x20的处理函数设置为timer_interrupt
4. set_sti(); 使用sti指令开启中断

其中`set_intr_gate`是从linux0.12的`system.h`文件中抠出来的宏，是几段嵌入汇编，把它放在了`gate_tool.h`头文件中

	#ifndef GATE_TOOL_H
	#define GATE_TOOL_H
	
	typedef struct desc_struct {
		unsigned long a,b;
	} desc_table[256];
	
	extern desc_table idt;
	
	#define _set_gate(gate_addr,type,dpl,addr) \
	__asm__ ("movw %%dx,%%ax\n\t" \
		"movw %0,%%dx\n\t" \
		"movl %%eax,%1\n\t" \
		"movl %%edx,%2" \
		: \
		: "i" ((short) (0x8000+(dpl<<13)+(type<<8))), \
		"o" (*((char *) (gate_addr))), \
		"o" (*(4+(char *) (gate_addr))), \
		"d" ((char *) (addr)),"a" (0x00080000))
	
	#define set_intr_gate(n,addr) \
		_set_gate(&idt[n],14,0,addr)
	
	#define set_trap_gate(n,addr) \
		_set_gate(&idt[n],15,0,addr)
	
	#define set_system_gate(n,addr) \
		_set_gate(&idt[n],15,3,addr)
	#endif

### 关于sti指令的一个疑点

我们从boot.s开始初始化系统的时候执行了cli指令，用来将cpu的eflags寄存器中的是否允许中断标志位置空，
从而屏蔽外部中断
现在我们初始化好了中断相关的环境了，需要开启中断，但是在《linux源码剖析》的第四章中我们看到

	48 pushfl
	49 andl $0xffffbfff, (%esp)
	50 popfl
	51 movl $TSS0_SEL, %eax
	52 ltr %ax
	53 movl $LDT0_SEL, %eax
	54 lldt %ax
	55 movl $0, current
	56 sti
	57 pushl $0x17
	58 pushl $init_stack
	59 pushfl
	60 pushl $0x0f
	61 pushl $task0
	62 iret

这样的代码
这段代码大概的意思是：已手动构造好了两个用户态任务的tss和堆栈，现在准备通过iret指令跳转到一个任务中
并且使用sti开启已经设置好的时钟中断，在时钟中断处理函数中来自动的调度两个任务
这里面的疑惑是：
	如果在sti之后iret之前时钟中断就来的话，逻辑不就有问题？因为现在的代码不在任何一个任务中，而切换
	任务的前提是已经在一个用户态任务中。
	另外我们又不能先iret到一个用户态程序中再使用sti开启中断，因为，第一这不是这个任务该做的事情，第二
	用户态程序也不能执行sti指令

我们在intel的开发文档中找到了[答案](https://www.felixcloutier.com/x86/sti)：
	
	 The delayed effect of this instruction is provided to allow interrupts to be enabled
	 just before returning from a procedure or subroutine. For instance, if an STI 
	 instruction is followed by an RET instruction, the RET instruction is allowed to 
	 execute before external interrupts are recognized. No interrupts can be recognized 
	 if an execution of CLI immediately follow such an execution of STI.
	
翻译过来
	
	提供此指令的延迟效果是为了允许正好在从过程或子例程返回之前启用中断。例如，如果STI指令后跟RET指令，
	则允许在识别外部中断之前执行RET指令。如果在这样的STI执行之后立即执行CLI，则不能识别任何中断。

就是说如果有一个ret指令在sti之后，则必定是先执行这个ret指令（或者iret），再可能触发中断。

### 运行效果
	
![](https://raw.githubusercontent.com/freelw/LanOS/master/demos/pic/init_8259a_succ.png)

	
	






