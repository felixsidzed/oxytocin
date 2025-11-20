[bits 16]
[org 0x7E00]

stage2:
	cli
	lgdt [gdtr32]

	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp 0x08:pm

%include "boot/gdt.inc"

[bits 32]
pm:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov ebp, 0x90000
	mov esp, ebp

	mov eax, cr4
	or eax, 0x600
	mov cr4, eax

	mov eax, cr0
	or eax, 0x2
	mov cr0, eax

	pushad
	pushfd
	pop eax
	mov ecx, eax
	xor eax, 1 << 21
	push eax
	popfd
	pushfd
	pop eax
	push ecx
	popfd
	cmp eax, ecx
	je .err

	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000001
	jb .err

	mov eax, 0x80000001
	cpuid
	test edx, 1 << 29
	jz .err

	mov edi, 0x1000
	mov cr3, edi
	xor eax, eax
	mov ecx, 4096
	rep stosd
	
	mov edi, cr3

	mov dword [edi], 0x2003
	add edi, 0x1000
	mov dword [edi], 0x3003
	add edi, 0x1000
	mov dword [edi], 0x4003
	add edi, 0x1000

	mov ebx, 0x3
	mov ecx, 512
.loop:
	mov dword [edi], ebx
	add ebx, 0x1000
	add edi, 8

	dec ecx
	jnz .loop
	
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	or eax, 1 << 11
	wrmsr

	lgdt [gdtr64]

	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax

	mov     eax, cr0
	or      eax, 0x00010000
	mov     cr0, eax 
	
	jmp 0x08:lm

.err:
	mov esi, err
	mov edx, 0xB8000
.err.loop:
	mov al, byte [esi]
	test al, al
	jz .err.done

	mov ah, 0x07
	mov word [edx], ax

	inc esi
	add edx, 2

	jmp .err.loop
.err.done:
	jmp $

[bits 64]
lm:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	jmp KERNEL

KERNEL equ 0x8200

err db "ERRHARDWARE", 0

times 512-($-lm) db 0
