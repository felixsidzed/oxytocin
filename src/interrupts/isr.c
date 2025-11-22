#include "isr.h"

#include <panic.h>
#include <string.h>
#include <oxystatus.h>

#include "idt.h"
#include "mmu/pmm.h"
#include "vga/vga.h"
#include "drivers/io.h"
#include "proc/process.h"

static ISR oxy_data handlers[256];

void isr_init() {
	memset(handlers, 0, sizeof(handlers));
	
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x00);
	outb(0xA1, 0x00);
}

void isr_register(uint8_t n, ISR f) {
	handlers[n] = f;
}

extern void scheduler_step(ISRContext* ctx);

void isr_handler(ISRContext* ctx) {
	ISR f = handlers[ctx->interruptNumber];
	if (f)
		f(ctx);
	else {
		Process* proc = process_getCurrent();
		if (proc->id) {
			// TODO: Log this
			// TODO: Call handler if available

			// we cant use process_exit() because it expects pit to be active
			proc->state = PROCESS_STATE_ZOMBIE;
			switch (ctx->interruptNumber) {
				case 0x00:
					proc->ec = STATUS_DIVIDE_BY_ZERO;
					break;
				case 0x01:
					proc->ec = STATUS_DBG_SINGLESTEP;
					break;
				case 0x03:
					proc->ec = STATUS_DBG_BREAKPOINT;
					break;
				case 0x04:
					proc->ec = STATUS_OVERFLOW;
					break;
				case 0x06:
					proc->ec = STATUS_INVALID_OPCODE;
					break;
				case 0x07:
					proc->ec = STATUS_NO_COPROCESSOR;
					break;
				case 0x09:
					proc->ec = STATUS_COPROC_OVERRUN;
					break;
				case 0x0A:
					proc->ec = STATUS_INVALID_TSS;
					break;
				case 0x0B:
					proc->ec = STATUS_MISSING_SEGMENT;
					break;

				case 0x0E:
					proc->ec = STATUS_ACCESS_VIOLATION;
					break;

				default:
					proc->ec = -1;
					break;
			}
			scheduler_step(ctx);
		} else
			kpanic_ctx(ctx, "Unhandled exception: 0x%llx", ctx->interruptNumber);
	}
}

void irq_handler(ISRContext* ctx) {
	if (ctx->interruptNumber >= 40)
		outb(0xA0, 0x20);
	outb(0x20, 0x20);
	
	ISR f = handlers[ctx->interruptNumber];
	if (f) f(ctx);
}
