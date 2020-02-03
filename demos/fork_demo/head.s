[BITS 32]

LATCH equ 11930
SCRN_SEL equ 0x18
TSS0_SEL equ 0x20
LDT0_SEL equ 0x28

global write_char, open_a20, gdt, idt, init_latch, init_8259A, timer_interrupt, page_fault
global assign_cr3_cr0, system_call, set_ldt_desc, sys_fork
extern lan_main, do_timer, sys_call_table, copy_process

global _e0, _e1, _e2, _e3, _e4, _e5, _e6, _e7, _e8, _e9, _e10, _e11, _e12, _e13, _e14, _e15, _e16
extern e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15, e16

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
    mov dword eax, TSS0_SEL
    ltr ax
    ;mov dword eax, LDT0_SEL
    ;lldt ax
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
    push byte ds
    push byte es
    push byte fs
    push dword eax
    push dword edx
    push dword ecx
    push dword ebx
    mov dword edx, 0x10
    mov ds, dx
    mov es, dx
    mov dword edx, 0x17
    mov fs, dx
    mov byte al, 0x20
    out byte 0x20, al
    call do_timer
    pop dword ebx
    pop dword ecx
    pop dword edx
    pop dword eax
    pop byte fs
    pop byte es
    pop byte ds
    iret

assign_cr3_cr0:
    mov dword eax, [esp+4]
    mov dword cr3, eax
    mov dword eax, cr0
    or dword eax, 0x80000000
    mov dword cr0, eax
    ret

page_fault:
    iret

system_call:
    push byte ds
    push byte es
    push byte fs
    push dword eax
    push dword edx
    push dword ecx
    push dword ebx
    mov dword edx, 0x10
    mov ds, dx
    mov es, dx
    mov dword edx, 0x17
    mov fs, dx
    call [sys_call_table+eax*4]
    push dword eax
    pop dword eax
    pop dword ebx
    pop dword ecx
    pop dword edx
    add dword esp, 4
    pop byte fs
    pop byte es
    pop byte ds
    iret

sys_fork:
    push gs
    push dword esi
    push dword edi
    push dword ebp
    call copy_process
    add dword esp, 16
    ret
set_ldt_desc:   ; set_tss_desc(n,addr)
    push dword eax
    push dword ebx
    mov ebx, [esp+12] ; n
    mov eax, [esp+16] ; addr
    mov word [ebx], 104
    mov word [ebx+2], ax
    ror dword eax, 16
    mov byte [ebx+4], al
    mov byte [ebx+5], 0xe2
    mov byte [ebx+6], 0x0
    mov byte [ebx+7], ah
    pop dword ebx
    pop dword eax
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
    dw 0x68, tss0, 0xe900, 0x0
    ;dw 0x40, ldt0, 0xe200, 0x0
    times 64 dq 0
end_gdt:

ldt0:
    dq 0x0000000000000000
    dq 0x00c0fa00000003ff
    dq 0x00c0f200000003ff

tss0:
    dd 0
    dd krn_stk0, 0x10
    dd 0, 0, 0, 0, 0
    dd 0, 0, 0, 0, 0
    dd 0, 0, 0, 0, 0
    dd 0, 0, 0, 0, 0, 0
    dd LDT0_SEL, 0x8000000

    times 128 dd 0
krn_stk0:

    times 128 dd 0
init_stack:         ;从这里开始是一个48位操作数
    dd init_stack   ;32位代表初始的esp
    dw 0x10         ;16位栈的段选择符，lss之后会加载到ss中


_e0:
    call e0
    iret
_e1:
    call e1
    iret
_e2:
    call e2
    iret

_e3:
    call e3
    iret

_e4:
    call e4
    iret
_e5:
    call e5
    iret
_e6:
    call e6
    iret
_e7:
    call e7
    iret
_e8:
    call e8
    iret
_e9:
    call e9
    iret
_e10:
    call e10
    iret
_e11:
    call e11
    iret
_e12:
    call e12
    iret
_e13:
    call e13
    iret
_e14:
    call e14
    iret

_e15:
    call e15
    iret
_e16:
    call e16
    iret
