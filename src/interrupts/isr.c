#include "isr.h"

#include "idt.h"

#include "std/mem.h"
#include "std/panic.h"

#include "vga/vga.h"

#include "drivers/io.h"

#include "physmem/physmem.h"

static ISR oxy_data handlers[256];

void isr_init() {
	memset(handlers, 0, sizeof(handlers));
	
	outb(PIC1_CMD, 0x11);
	outb(PIC2_CMD, 0x11);
	outb(PIC1_DATA, 0x20);
	outb(PIC2_DATA, 0x28);
	outb(PIC1_DATA, 0x04);
	outb(PIC2_DATA, 0x02);
	outb(PIC1_DATA, 0x01);
	outb(PIC2_DATA, 0x01);
	outb(PIC1_DATA, 0x00);
	outb(PIC2_DATA, 0x00);
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
		outb(PIC2_CMD, PIC_EOI);
	outb(PIC1_CMD, PIC_EOI);
	
	ISR f = handlers[ctx->interruptNumber];
	if (f) f(ctx);
}
