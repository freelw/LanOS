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