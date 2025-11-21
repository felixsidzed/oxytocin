[bits 16]
[org 0x7C00]

boot:
	; load 2nd stage and kernel from disk
	mov bx, 2
	mov ah, 0x42
	mov si, packet
	int 0x13
	jc .err

	; jump to 2nd stage
	jmp 0x0000:0x7E00

.err:
	mov si, err
	mov ah, 0x0E
.loop:
	lodsb
	test al, al
	jz .done
	int 0x10
	jmp .loop
.done:
	jmp $

packet:
	db 16		; sizeof packet
	db 0		; reserved
	dw 0x67		; size (see build.py, line 109)
	dw 0x7E00	; buffer offset
	dw 0		; buffer segment
	dq 1		; starting lba

err db 'ERRDISK', 0
drive: db 0x00

times 510-($-$$) db 0
dw 0xAA55
