#pragma once

typedef struct {
	uint64_t R15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t Rdi, rsi, rbp, rbx, rdx, rcx, rax;
	uint64_t interruptNumber, errorCode;
	uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed)) Context;

#define PIC1_CMD	0x20
#define PIC1_DATA	0x21
#define PIC2_CMD	0xA0
#define PIC2_DATA	0xA1
#define PIC_EOI		0x20

typedef void (*ISR)(Context* ctx);

void isr_init();
void isr_register(uint8_t n, ISR handler);
