#include "panic.h"

#include "vga/vga.h"

#include <stdarg.h>
#include <string.h>
#include <kprintf.h>

#include "drivers/io.h"
#include "drivers/keyboard.h"

oxy_noret kvpanic(Context* ctx, const char* message, va_list va) {
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
	Context ctx;
    asm volatile(
        "mov %%r15, 0x00(%0)\n"
        "mov %%r14, 0x08(%0)\n"
        "mov %%r13, 0x10(%0)\n"
        "mov %%r12, 0x18(%0)\n"
        "mov %%r11, 0x20(%0)\n"
        "mov %%r10, 0x28(%0)\n"
        "mov %%r9,  0x30(%0)\n"
        "mov %%r8,  0x38(%0)\n"
        "mov %%rdi, 0x40(%0)\n"
        "mov %%rsi, 0x48(%0)\n"
        "mov %%rbp, 0x50(%0)\n"
        "mov %%rbx, 0x58(%0)\n"
        "mov %%rdx, 0x60(%0)\n"
        "mov %%rcx, 0x68(%0)\n"
        "mov %%rax, 0x70(%0)\n"

        "movq $0,  0x78(%0)\n"
        "movq $0,  0x80(%0)\n"

        "mov (%%rsp), %%rax\n"
        "mov %%rax, 0x88(%0)\n"

        "mov %%cs, %%rax\n"
        "mov %%rax, 0x90(%0)\n"

        "pushfq\n"
        "popq %%rax\n"
        "mov %%rax, 0x98(%0)\n"

        "mov %%rsp, 0xa0(%0)\n"

        "mov %%ss, %%rax\n"
        "mov %%rax, 0xa8(%0)\n"

        :
        : "r"(&ctx)
        : "rax", "memory"
    );

	va_list va;
	va_start(va, message);

	kvpanic(&ctx, message, va);
	oxy_unreachable();
}

oxy_noret kpanic_ctx(Context* ctx, const char* message, ...) {
	va_list va;
	va_start(va, message);

	kvpanic(ctx, message, va);
	oxy_unreachable();
}
