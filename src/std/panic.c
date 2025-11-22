#include "panic.h"

#include "vga/vga.h"

#include <stdarg.h>
#include <string.h>
#include <kprintf.h>

#include "drivers/io.h"
#include "proc/process.h"
#include "drivers/keyboard.h"

oxy_noret kvpanic(ISRContext* ctx, const char* message, va_list va) {
	asm volatile ("cli");

	vga_setColor(7, 0);
	vga_cls();

	puts("KERNEL PANIC:\n  ");

	if (message) {
		kvprintf(message, va);
		putchar('\n');
	}

	char buf[80];
	while (true) {
		putchar('>');
		size_t len = gets(buf, sizeof(buf));
		if (len == 0)
			continue;

		char* p = buf;

		if (isalpha(*p)) {
			char tmp[len];
			int i = 0;
			while (isalnum(*p))
				tmp[i++] = *p++;
			tmp[i] = '\0';

			if (streq(tmp, "reboot")) {
				while (!(inb(0x64) & 1)) {}
				outb(0x64, 0xFE);
			} else if (streq(tmp, "frame"))
				kprintf(
					"RIP: %p\tRSP: %p\nINT_NO: %u\tERR: %u\n",
					ctx->rip, ctx->rsp, ctx->interruptNumber, ctx->errorCode
				);
			else if (streq(tmp, "regs")) {
				kprintf(
					"RAX: %p  RBX: %p  RCX: %p  RDX: %p  RSI: %p  RDI: %p  RBP: %p\n",
					ctx->rax, ctx->rbx, ctx->rcx, ctx->rdx, ctx->rsi, ctx->rdi, ctx->rbp
				);
				kprintf(
					"R8: %p  R9: %p  R10: %p  R11: %p  R12: %p  R13: %p  R14: %p  R15: %p\n",
					ctx->r8, ctx->r9, ctx->r10, ctx->r11, ctx->r12, ctx->r13, ctx->r14, ctx->r15
				);

				uintptr_t cr[3];
				asm volatile("mov %%cr0, %0" : "=r"(cr[0]));
				asm volatile("mov %%cr2, %0" : "=r"(cr[1]));
				asm volatile("mov %%cr3, %0" : "=r"(cr[2]));
				kprintf(
					"CR0: %p\tCR2: %p\tCR3: %p\n",
					cr[0], cr[1], cr[2]
				);
			} else if (streq(tmp, "cls"))
				vga_cls();
			else if (streq(tmp, "traceback")) {
				uintptr_t* rbp = (uintptr_t*)ctx->rbp;
				int frame = 0;
				puts("(current) ");
				while (rbp) {
					kprintf("#%u %p\n", ++frame, rbp[1]);
					rbp = (uintptr_t*)rbp[0];

					if ((uintptr_t)rbp < 0x1000 || (uintptr_t)rbp > 0xFFFFFFFFFFFF)
						break;
				}
			} else if (streq(tmp, "stack")) {
				uintptr_t* rsp = (uintptr_t*)ctx->rsp;
				uintptr_t* rbp = (uintptr_t*)ctx->rbp;

				if (rsp > rbp)
					kprintf("invalid stack\n");
				else {
					int i = 0;
					for (uintptr_t* p = rsp; p <= rbp; p++)
						kprintf("#%d %p: 0x%p\n", ++i, p, *p);
				}
			} else
				puts("invalid command\n");
		}
	}

	while (true)
		asm volatile ("cli\nhlt");
}

oxy_noret kpanic(const char* message, ...) {
	ISRContext ctx;
	Context* pctx = &process_getCurrent()->ctx;
	memcpy(&ctx, pctx, sizeof(uintptr_t)*15); // copy general purpose registers

	ctx.interruptNumber = ctx.errorCode = 0;

	ctx.rip = pctx->rip;
	ctx.rsp = pctx->rsp;
	ctx.rflags = pctx->rflags;

	va_list va;
	va_start(va, message);

	kvpanic(&ctx, message, va);
	oxy_unreachable();
}

oxy_noret kpanic_ctx(ISRContext* ctx, const char* message, ...) {
	va_list va;
	va_start(va, message);

	kvpanic(ctx, message, va);
	oxy_unreachable();
}
