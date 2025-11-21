#include "isr.h"

#include "idt.h"

#include <string.h>
#include "std/panic.h"

#include "vga/vga.h"

#include "drivers/io.h"

#include "mmu/pmm.h"

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

void isr_handler(Context* ctx) {
	ISR f = handlers[ctx->interruptNumber];
	if (f)
		f(ctx);
	else {
		kpanic_ctx(ctx, "Unhandled exception: 0x%llx", ctx->interruptNumber);
	}
}

void irq_handler(Context* ctx) {
	if (ctx->interruptNumber >= 40)
		outb(0xA0, 0x20);
	outb(0x20, 0x20);
	
	ISR f = handlers[ctx->interruptNumber];
	if (f) f(ctx);
}
