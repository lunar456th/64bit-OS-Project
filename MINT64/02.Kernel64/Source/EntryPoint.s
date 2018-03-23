[BITS 64]

SECTION .text
extern main ; use outer function

START:
	mov ax, 0x10 ; 0x10 = IA-32e data segment descriptor.
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; stack 0x600000 ~ 0x6FFFFF
	mov ss, ax
	mov rsp, 0x6FFFF8
	mov rbp, 0x6FFFF8

	call main

	jmp $
