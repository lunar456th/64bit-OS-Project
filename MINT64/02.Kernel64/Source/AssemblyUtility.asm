[BITS 64]

SECTION .text

global kInPortByte, kOutPortByte, kLoadGDTR, kLoadTR, kLoadIDTR ; C언어에서 이 함수들을 쓸 수 있게끔 global해줌
global kEnableInterrupt, kDisableInterrupt, kReadRFLAGS
global kReadTSC

kInPortByte: ; param - rdi = 파라미터로 port주소를 받아서 갖고 있음
	push rdx ; save rdx to use rdx in this function

	mov rdx, rdi ; rdx <- portnumber
	mov rax, 0 ; init rax
	in al, dx ; dx레지스터에 저장된 포트 주소에서 한바이트를 읽어서 AL에 저장.

	pop rdx ; restore
	ret

kOutPortByte: ; param - rdi = parameter1(port), rsi = parameter2(data)
	push rdx
	push rax

	mov rdx, rdi
	mov rax, rsi
	out dx, al ; al -> [dx]

	pop rax
	pop rdx
	ret

kLoadGDTR:
	lgdt [rdi]
	ret

kLoadTR:
	ltr di
	ret

kLoadIDTR:
	lidt [rdi]
	ret

kEnableInterrupt:
	sti
	ret

kDisableInterrupt:
	cli
	ret

kReadRFLAGS:
	pushfq
	pop rax

	ret

kReadTSC:
	push rdx
	rdtsc

	shl rdx, 32
	or rax, rdx

	pop rdx
	ret

