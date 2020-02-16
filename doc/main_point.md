从知识的两端向中间对接

1.从我们使用的linux的印象
多进程
不能直接操作硬件
系统调度硬件

2.从裸机
如何使用硬件
目前控制 cpu 和内存
取地址 e ip

gcc--> gas --> elf .o 
nasm --> elf .o

nasm
mov ax, [0xaa]
mov ax, 0xaa

gas 
*0xaa --> ax
    mov 0xaa, %ax
0xaa -> ax
    mov $0xaa, %ax
nasm:
    mov ax, 1 --> ea bb cc
gas:
    mov 1, %ax -->  ea bb cc
0x0 mov ax 1
ip++
0x3

push ax --> ax --> *esp; esp -= 4
pop ax --> *esp --> ax esp += 4

1 print x
2 call func    *esp = 3 esp -= 4
3 print eax

head.s --> head.o
push ebx
push eax
call copy_process

fork.c --> fork.o
int copy_process(int eax, int ebx)
{
    print eax
}
5 func
    print y 
    ret
执行
改变寄存器状态， 改变内存的值（包括显存，统一编地址）
eax ebx 。。。。
esp
ds es gs 。。。 ss

ax ds word 16 bit
1M -> 1024*1024
2^20

20根地址线
[ds:ax] = (ds << 4) + ax

es = 0x1000
bx = 0xff
[es:bx] = 0x10000 + 0xff = 0x10ff

*(0x10ff) = 1
ax 1
mov [es:bx], ax
ax 16 al low ah high
out 0x20, al
in al, 0x20

0 1 2 3 out 
特点：顺序执行，高权限
裸机启动
16位实模式， 访问1M内存

32位保护模式 4* 1024 1024 1024
顺序高权限
段级别硬件支持
需要提前布局一些内存信息，intel的一堆规则
可以访问4G

[ds:ax]

[0x8:0x1] = 0x81
ax  eax
16->32
ds ax--> extend ax  eax
0x8

int 13
：
print sigv 13
gdt
0:
[
    8字节，0x0
    8字节，0x8 代码段
    【--------------】 基地址，限长，特权级 
    【--------------】
    8字节，0x10 数据段
    8字节，
    8字节，
]
gdtr 

cr0 p -->1 
cs -> 0x8

jmp 0x8, 0 -> 
1. code seg = 0x8
2. eip = 0


lgdt 48bit
[16 32]

分页机制和用户态，提供一个逻辑上的4G空间，有限功能

0 16 ---> 0 32 ---> 3 32 -->

call xxxxx

int 0x80
gdt
TSS 104

esp0 ss0 0


ss           esp
stack seg    extend stack pointer
cs           eip
code seg     extend instructions pointer

write -->  100
mov ax, 100
read 101
int 0x80

call_bak:

interrupt

ss 0x10 


系统调度多任务
时钟 + 分割好的内存 + 给自的内核栈

a  cs eip

100

101

int 0x20

....

200

b cs eip

3000

int 0x20
...

int 0x20
3220


fork

用户栈和内核栈
特权级切换会切换栈 int iret




intel 
























1. 内存布局

    1. elf结构
    2. 拼接过程

2. 启动顺序

3. 部件列表

    1.时钟
    2.页表
    3.保护模式

4. 调试方法