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
