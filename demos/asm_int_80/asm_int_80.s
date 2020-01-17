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
