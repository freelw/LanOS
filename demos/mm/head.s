[BITS 32]

LATCH equ 11930
SCRN_SEL equ 0x18
TSS0_SEL equ 0x20
LDT0_SEL equ 0x28
TSS1_SEL equ 0x30
LDT1_SEL equ 0x38
global write_char, open_a20, idt, init_latch, init_8259A, timer_interrupt, set_sti
global assign_cr3_cr0
extern lan_main, do_timer
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
    mov eax, [esp+16]
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

open_a20:
    call empty_8042
    mov al, 0xd1
    out 0x64, al
    call empty_8042
    mov al, 0xdf
    out 0x60, al
    call empty_8042
    ret

empty_8042:
    dw 0x00eb
    dw 0x00eb
    in al, 0x64
    test al, 0x2
    jnz empty_8042
    ret

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

timer_interrupt:
    mov al, 0x20
    out 0x20, al
    call do_timer
    iret

set_sti:
	sti
    ret

assign_cr3_cr0:
    mov dword eax, [esp+4]
    mov dword cr3, eax
    mov dword eax, cr0
    or dword eax, 0x80000000
    mov dword cr0, eax
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
