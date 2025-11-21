[bits 16]
[org 0x7E00]

stage2:
	; disable interrupts, for now, because we dont have an idt
	cli
	lgdt [gdtr32]

	; enable protected mode
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

	; enable PAE + PGE
	mov eax, cr4
	or  eax, 0x600
	mov cr4, eax

	; enable coprocessor shit
	mov eax, cr0
	or  eax, 0x2
	mov cr0, eax

	; read eflags into eax
	pushfd
	pop eax
	mov ecx, eax
	; toggle 'cpuid supported' flag
	xor eax, 1 << 21
	push eax
	; write modified flags
	popfd
	; read eflags back
	pushfd
	pop eax
	; restore eflags
	push ecx
	popfd
    ; unchanged = no cpuid
	cmp eax, ecx
	je .err

	; check if extended function 0x80000001 is supported
	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000001
	jb .err

	; get extended feature flags
	mov eax, 0x80000001
	cpuid
	; check for long mode support
	test edx, 1 << 29
	jz .err

	; set up page table
	mov edi, 0x1000
	mov cr3, edi

	xor eax, eax
	mov ecx, 4096
	rep stosd
	
	mov edi, cr3

	; set page table entries
	mov dword [edi], 0x2003	; present | writeable
	add edi, 0x1000
	mov dword [edi], 0x3003	; present | writeable
	add edi, 0x1000
	mov dword [edi], 0x4003	; present | writeable
	add edi, 0x1000

	; 512 pages
	mov ebx, 0x3
	mov ecx, 512
.loop:
	mov dword [edi], ebx
	add ebx, 0x1000
	add edi, 8
	
	dec ecx
	jnz .loop

	; enable physical address extension
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8				; enable no execute thing
	or eax, 1 << 11				; enable long mode
	wrmsr

	lgdt [gdtr64]

	; enable paging
	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax

	; set write protect
	mov eax, cr0
	or eax, 0x00010000
	mov cr0, eax 
	
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

err db "BADHARDWARE", 0

times 512-($-lm) db 0
