#pragma once

typedef struct ISRContext {
	uintptr_t r15, r14, r13, r12, r11, r10, r9, r8;
	uintptr_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
	
	uintptr_t interruptNumber, errorCode;
	uintptr_t rip, cs, rflags, rsp, ss;
} __attribute__((packed)) ISRContext;

typedef void (*ISR)(ISRContext* ctx);

/// @brief Initializes the interrupt service routine system
void isr_init();

/// @brief Registers an interrupt handler for a specific interrupt number
/// @param n Interrupt number to handle
/// @param handler Function pointer to the ISR
void isr_register(uint8_t n, ISR handler);
