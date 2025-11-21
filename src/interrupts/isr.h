#pragma once

/// TODO: We should save the *entire* CPU state
typedef struct {
#if defined(__x86_64__) || defined(_M_X64)
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
#endif
	uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
	uint64_t interruptNumber, errorCode;
	uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed)) Context;

typedef void (*ISR)(Context* ctx);

/// @brief Initializes the interrupt service routine system
void isr_init();

/// @brief Registers an interrupt handler for a specific interrupt number
/// @param n Interrupt number to handle
/// @param handler Function pointer to the ISR
void isr_register(uint8_t n, ISR handler);
