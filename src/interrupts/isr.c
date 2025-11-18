#include "isr.h"

#include "idt.h"
#include "std/mem.h"
#include "vga/vga.h"
#include "drivers/io.h"

static ISR oxy_nobss handlers[256];

static const char* exceptionMessages[32] = {
	"EXCEPTION_DIVIDE_BY_ZERO",
	"EXCEPTION_DEBUG",
	"EXCEPTION_NON_MASKABLE_INTERRUPT",
	"EXCEPTION_BREAKPOINT",
	"EXCEPTION_OVERFLOW",
	"EXCEPTION_BOUND_RANGE_EXCEEDED",
	"EXCEPTION_INVALID_OPCODE",
	"EXCEPTION_NO_COPROCESSOR",
	"EXCEPTION_DOUBLE_FAULT",
	"EXCEPTION_COPROCESSOR_SEGMENT_OVERRUN",
	"EXCEPTION_INVALID_TSS",
	"EXCEPTION_SEGMENT_NOT_PRESENT",
	"EXCEPTION_STACK_FAULT",
	"EXCEPTION_GENERAL_PROTECTION_FAULT",
	"EXCEPTION_PAGE_FAULT",
	"EXCEPTION_UNKNOWN_INTERRUPT",
	"EXCEPTION_COPROCESSOR_ERROR",
	"EXCEPTION_ALIGNMENT_CHECK",
	"EXCEPTION_MACHINE_CHECK",
	"EXCEPTION_RESERVED_19",
	"EXCEPTION_RESERVED_20",
	"EXCEPTION_RESERVED_21",
	"EXCEPTION_RESERVED_22",
	"EXCEPTION_RESERVED_23",
	"EXCEPTION_RESERVED_24",
	"EXCEPTION_RESERVED_25",
	"EXCEPTION_RESERVED_26",
	"EXCEPTION_RESERVED_27",
	"EXCEPTION_RESERVED_28",
	"EXCEPTION_RESERVED_29",
	"EXCEPTION_RESERVED_30",
	"EXCEPTION_RESERVED_31"
};

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

static const char hexDigits[] = "0123456789ABCDEF";

void isr_handler(Context* ctx) {
	ISR f = handlers[ctx->interruptNumber];
	if (f)
		f(ctx);
	else {
		vga_setColor(15, 1);
		vga_cls();

		puts(
			"\n\n\n\n\n\n\n"
			"                                     ;C\n"
			"\n"
			"     Your PC ran into a problem and needs to restart.\n"
			"     We're just collecting some error info, and then we'll restart for you.\n"
			"     If you'd like to know more, you can search online later for this error:\n"
			"\n\n"
			"     "
		);
		puts(exceptionMessages[ctx->interruptNumber]);
		putchar(' '), putchar('(');
		putchar('0'), putchar('x');
		for (int i = 28; i >= 0; i -= 4)
			putchar(hexDigits[(ctx->interruptNumber >> i) & 0xF]);
		putchar(')');

		asm("hlt");
	}
}

void irq_handler(Context* ctx) {
	if (ctx->interruptNumber >= 40)
		outb(PIC2_CMD, PIC_EOI);
	outb(PIC1_CMD, PIC_EOI);
	
	ISR f = handlers[ctx->interruptNumber];
	if (f) f(ctx);
}
