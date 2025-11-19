gdt32Start:
	dq 0					; null
	dq 0x00CF9A000000FFFF	; cs 32
	dq 0x00CF92000000FFFF	; ds 32
gdt32End:

gdt64Start:
	dq 0					; null
	dq 0x00AF9A000000FFFF	; cs 64
	dq 0x00AF92000000FFFF	; ds 64
gdt64End:

gdtr32:
	dw gdt32End - gdt32Start - 1
	dd gdt32Start

gdtr64:
	dw gdt64End - gdt64Start - 1
	dd gdt64Start
