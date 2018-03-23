[ORG 0x00] ; start address of code is 0x00.
[BITS 16] ; the following codes are 16-bits.

SECTION .text ; text section define

jmp 0x07C0:START

TOTALSECTORCOUNT: dw 0x02 ; total size of OS image (in case of 1024, 0x90000)

KERNEL32SECTORCOUNT: dw 0x02 ; total # of sector of protected mode kernel

START:
	mov ax, 0x07C0 ; 0x07C0 is the start address of bootloader
	mov ds, ax
	mov ax, 0xB800
	mov es, ax ; es also is one of data segment register

	mov ax, 0x0000
	mov ss, ax ; stack segment
	mov sp, 0xFFFE
	mov bp, 0xFFFE

	mov si, 0 ; string register initialize

.SCREENCLEANLOOP:
	mov byte[es:si], 0 ; initalization of video memory
	mov byte[es:si + 1], 0x0A

	add si, 2 ; move position

	cmp si, 80 * 25 * 2
	jl .SCREENCLEANLOOP ; initiate video memory while in loop

	; print start message
	push MESSAGE1
	push 0 ; y pos
	push 0 ; x pos
	call PRINTMESSAGE ; print
	add sp, 6 ; remove inserted parameter

	; print Loading Message
	push IMAGELOADINGMESSAGE
	push 1
	push 0
	call PRINTMESSAGE
	add sp, 6

	; image loading
	; reset before reading
RESETDISK:
	; BIOS reset function
	mov ax, 0 ; ax has BIOS service number. 0 is reset.
	mov dl, 0 ; dl is for driver number. floppy driver is 0.
	int 0x13 ; 0x13 is disk I/O.

	jc HANDLEDISKERROR ; jc is jmp if carry set and it is for handling error here.

	; read sector from disk
	mov si, 0x1000
	mov es, si
	mov bx, 0x0000 ; 0x1000:0000 = 0x10000 and it is start address of OS image to position
	mov di, word[TOTALSECTORCOUNT] ; indirect address mode. word[TOTAL...] = 1024 words which is the size of OS image

READDATA:
	cmp di, 0 ; di = 1024 ~~~> 0
	je READEND ; until reading is complete
	sub di, 0x1 ; dec 1 from di

	; call BIOS Read function
	mov ah, 0x02 ; service number = 2
	mov al, 0x1 ; # of sector to read is 1 - originally able to read multiple sectors but we doesn't
	mov ch, byte[TRACKNUMBER] ; ch = no. of track to read
	mov cl, byte[SECTORNUMBER] ; cl = no. of sector to read
	mov dh, byte[HEADNUMBER] ; dh = no. of head to read
	mov dl, 0x00 ; dl = no. of drive to read (0 = floppy)
	int 0x13
	jc HANDLEDISKERROR ; it seems that when occurring error carry bit is set.

	; after read, recalculate addresses
	add si, 0x0020 ; 0x200 = 512 byte
	mov es, si
	mov al, byte[SECTORNUMBER]
	add al, 0x01 ; inc sector number
	mov byte[SECTORNUMBER], al ; these 3 stmt make SECTORNUMER to have sector number to read next.
	cmp al, 19
	jl READDATA

	; if al = 19 then it is that the last sector was read.
	; reset sector number to 1
	xor byte[HEADNUMBER], 0x01 ; header number is toggled. 0->1->0->1->...
	mov byte[SECTORNUMBER], 0x01 ; reset sector number to 1
	cmp byte[HEADNUMBER], 0x00 ; when head number 1->0, it is that both heads are all read. so move 1 down and inc track number 1
	jne READDATA
	add byte[TRACKNUMBER], 0x01
	jmp READDATA

READEND:
	; print message that OS image has been loaded completely.
	push LOADINGCOMPLETEMESSAGE ; address to print
	push 1 ; y pos
	push 20 ; x pos
	call PRINTMESSAGE
	add sp, 6 ; remove inserted parameter

	; execute loaded OS image
	jmp 0x1000:0x0000

; area of functions
HANDLEDISKERROR:
	push DISKERRORMESSAGE ; disk error message
	push 1 ; y
	push 20 ; x
	call PRINTMESSAGE ; call print message

	jmp $ ; infinite loop

PRINTMESSAGE:
	push bp ; put registers into stack
	mov bp, sp ; access paramater using bp.
	push es
	push si
	push di
	push ax
	push cx
	push dx

	mov ax, 0xB800
	mov es, ax ; es has video memory

	mov ax, word[bp + 6] ; xpos, ypos, message was in stack and sp(bp) was pointing them. bp + 6 is ypos.
	mov si, 160 ; 80 columns * 2 bytes = row num
	mul si
	mov di, ax

	mov ax, word[bp + 4] ; bp + 4 = xpos
	mov si, 2 ; 2 bytes -> column num
	mul si

	add di, ax ; accurate position to print

	mov si, word[bp + 8] ; bp + 8 = message start position

.MESSAGELOOP:
	mov cl, byte[si]
	cmp cl, 0
	je .MESSAGEEND ; goto .MESSAGEEND after string copy. if cl = 0 then exit because I appended zero the end of each string.

	mov byte[es:di], cl ; char(cl) -> video(es:di) print
	add si, 1 ; char inc
	add di, 2 ; video memory(byte) inc
	jmp .MESSAGELOOP

.MESSAGEEND:
	pop dx
	pop cx
	pop ax
	pop di
	pop si
	pop es
	pop bp
	ret ; return to where calls the function.

MESSAGE1: db 'MINT64 OS Boot Loader Start~~!!', 0 ; like an array, string and zero is saved sequentially. And when to save string, use db(byte) since byte is 1 byte.

DISKERRORMESSAGE: db 'DISK Error~!!', 0
IMAGELOADINGMESSAGE: db 'OS Image Loading...', 0
LOADINGCOMPLETEMESSAGE: db 'Complete~!!', 0

SECTORNUMBER: db 0x02
HEADNUMBER: db 0x00
TRACKNUMBER: db 0x00

; fill the remaining space with 0x00
times 510 - ($ - $$) db 0x00 ; $ is address of current line
							 ; $$ is address of current section
							 ; input 0x00(which is size of byte) into 0~510 address

db 0x55 ; input 0x55 into 511
db 0xAA ; input 0xAA into 512, these two lines are the signature of boot sector.



;
;mov ax, 0xB800 ; 0xB800 is the start address of IBM PC's VGA memory
;mov ds, ax ; data segment points 0xB800
;
;mov byte [0x00], 'M' ; [0x00] is equal to 'ds:0x00'
;mov byte [0x01], 0x4A ; the attribute of 'M' is into ds+0x01

;jmp $
