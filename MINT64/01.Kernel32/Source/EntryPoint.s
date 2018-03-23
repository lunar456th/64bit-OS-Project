[ORG 0X00]
[BITS 16]

SECTION .text

START:
	mov ax, 0x1000 ; convert the start address of protected mode(0x10000) to the value of segment register.
	mov ds, ax
	mov es, ax

	; activate A20 gate using BIOS service
	mov ax, 0x2401 ; A20 activation service setting
	int 0x15 ; 0x15 = call BIOS interrupt service
	jc .A20GATEERROR ; if interrupt error occurs, carry is set.
	jmp .A20GATESUCCESS ;


.A20GATEERROR:
	; if error occurs, activate A20 gate using System Control Port.
	in al, 0x92 ; 0x92 is system control port. read 1 byte from 0x92 and store to AL register.
	or al, 0x02 ; set A20 Gate bit to 1 from AL
	and al, 0xFE ; set 0th bit to 0 to prevent system reset.
	out 0x92, al ; store modified value into 0x92(system control port)

.A20GATESUCCESS:
	cli ; interrupt disable
	lgdt [GDTR] ; load GDT table

	; enter protected mode
	mov eax, 0x4000003B ; put proper flag bits into CR0 register and enter protected mode
	mov cr0, eax

	jmp dword 0x18: (PROTECTEDMODE - $$ + 0x10000) ; 0x18 is the address in which code segment descriptor of protected mode is.

[BITS 32]
PROTECTEDMODE:
	mov ax, 0x20 ; 0x20 = data segment descriptor of protected mode.
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; create stack in range from 0x00000000 to 0x0000FFFF
	mov ss, ax
	mov esp, 0xFFFE
	mov ebp, 0xFFFE

	push (SWITCHSUCCESSMESSAGE - $$ + 0x10000) ; push message to print
	push 2 ; ypos
	push 0 ; xpos
	call PRINTMESSAGE
	add esp, 12 ; remove inserted parameter

	jmp dword 0x18: 0x10200 ; 0x08 = point cs segment register to kernel code descriptor(0x08) and jump to 0x10200

PRINTMESSAGE:
	push ebp
	mov ebp, esp
	push esi
	push edi
	push eax
	push ecx
	push edx

	mov eax, dword[ebp + 12] ; [msg/y/x/ebp_origin/ebp_now], each are 4 bytes. so ebp + 12 is y.
	mov esi, 160 ; (2(bytes) * 80(columns)) -> calculate y address.
	mul esi
	mov edi, eax

	mov eax, dword[ebp + 8] ; ebp + 8 is xpos.
	mov esi, 2 ; calculate x address.
	mul esi
	add edi, eax

	mov esi, dword[ebp + 16]

.MESSAGELOOP:
	mov cl, byte[esi]
	cmp cl, 0
	je .MESSAGEEND

	mov byte[edi + 0xB8000], cl ; print
	add esi, 1 ; next char
	add edi, 2 ; next byte
	jmp .MESSAGELOOP

.MESSAGEEND:
	pop edx
	pop ecx
	pop eax
	pop edi
	pop esi
	pop ebp
	ret

; data area
align 8, db 0 ; align following data in 8-byte size.

dw 0x0000 ; to align the end of GDT to 8-byte.

GDTR: ; GDTR structure definition. following 2 data are the component of GDTR.(it's like metadata)
	dw GDTEND - GDT - 1 ; whole size of following GDT table
	dd (GDT - $$ + 0x10000) ; start address of following GDT table

GDT: ; it has only two descriptor. (code segment descriptor, data segment descriptor)
	NULLDescriptor: ; null descriptor must exists in the start position of GDT table
		dw 0x0000
		dw 0x0000
		db 0x00
		db 0x00
		db 0x00
		db 0x00

	IA_32eCODEDESCRIPTOR: ; code segment descriptor of IA-32e
		dw 0xFFFF
		dw 0x0000
		db 0x00
		db 0x9A
		db 0xAF
		db 0x00

	IA_32eDATADESCRIPTOR: ; data segment descriptor of IA-32e
		dw 0xFFFF
		dw 0x0000
		db 0x00
		db 0x92
		db 0xAF
		db 0x00

	CODEDESCRIPTOR:
		dw 0xFFFF ; limit [15:0]
		dw 0x0000 ; base [15:0]
		db 0x00 ; base [23:16]
		db 0x9A ; P=1, DPL=0, code segment, execute/read
		db 0xCF ; G=1, D=1, L=0, limit[19:16]
		db 0x00 ; base[31:24]

	DATADESCRIPTOR:
		dw 0xFFFF ;
		dw 0x0000 ;
		db 0x00 ;
		db 0x92 ; , , data segment, read/write
		db 0xCF ;
		db 0x00 ;

GDTEND:

SWITCHSUCCESSMESSAGE: db 'Switch To Protected Mode Success~!!', 0

times 512 - ($ - $$) db 0x00 ; fill remained area with zero

