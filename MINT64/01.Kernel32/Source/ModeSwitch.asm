[BITS 32]

global kReadCPUID, kSwitchAndExecute64bitKernel ; export the name for C code to access this function.

SECTION .text

kReadCPUID:
	push ebp
	mov ebp, esp
	push eax
	push ebx
	push ecx
	push edx
	push esi

	; execute CPUID with EAX
	; param (which is from C code) : DWORD dwEAX DWORD * pdwEAX, * pdwEBX, * pdwECX, * pdwEDX
	mov eax, dword[ebp + 8] ; stack = pdwEDX/pdwECX/pdwEBX/pdwEAX/dwEAX/ebp/esp(=ebp)/eax/ebx/ecx/edx/esi. so ebp + 8(=4*2) = dwEAX.
	cpuid ; execute cpuid with dwEAX

	; store the value returned
	mov esi, dword[ebp + 12] ; pdwEAX
	mov dword[esi], eax ; EAX <- pdwEAX

	mov esi, dword[ebp + 16] ; pdwEBX
	mov dword[esi], ebx ; EBX <- pdwEBX

	mov esi, dword[ebp + 20] ; pdwECX
	mov dword[esi], ecx ; ECX <- pdwECX

	mov esi, dword[ebp + 24] ; pdwEDX
	mov dword[esi], edx ; EDX <- pdwEDX

	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax
	pop ebp
	ret

kSwitchAndExecute64bitKernel: ; change to IA-32e and execute 64bit kernel
	; set CR4 PAE bit to 1
	mov eax, cr4
	or eax, 0x20
	mov cr4, eax

	; activate PML4 table address and cache from CR3
	mov eax, 0x100000 ; 0x100000 is the location in which PML4 table is
	mov cr3, eax

	; set IA_32EFER.LME to 1 and activate IA-32e mode
	mov ecx, 0xC0000080 ; 0xC0000080 is the address of MSR register?
	rdmsr ; read msr

	or eax, 0x0100 ; set LME(bit 8) 1 from the lower 32 bits.

	wrmsr ; write msr

	; activate cache and paging by setting CR0 bits
	mov eax, cr0
	or eax, 0xE0000000 ; set bits
	xor eax, 0x60000000 ; set bits
	mov cr0, eax

	jmp 0x08:0x200000 ; 0x08 is the address in which code segment descriptor for IA-32e is.

	jmp $ ; not be executed
