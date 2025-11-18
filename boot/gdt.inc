gdtStart:
	dq 0					; null
	dq 0x00CF9A000000FFFF	; cs 32
	dq 0x00CF92000000FFFF	; ds 32
	dq 0x00AF9A000000FFFF	; cs 64
	dq 0x00AF92000000FFFF	; ds 64
gdtEnd:

gdtr:
	dw gdtEnd - gdtStart - 1
	dd gdtStart

CS32 equ 0x08
DS32 equ 0x10
CS64 equ 0x18
DS64 equ 0x20
