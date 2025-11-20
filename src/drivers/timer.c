#include "timer.h"

#include "drivers/io.h"

#include "vga/vga.h"

static oxy_data size_t ticks = 0;
static oxy_data uint32_t freq = 0;
static oxy_data uint8_t ticksSinceStep = 0;

void timer_heartbeat(Context* ctx) {
	if (ticksSinceStep >= 16) {
		//scheduler_step(ctx);
		ticksSinceStep = 0;
	}

	ticks++, ticksSinceStep++;
}

void timer_init(uint32_t _) {
	freq = _;

	uint32_t divisor = 1193182 / _;
	if (divisor == 0) divisor = 1;
	else if (divisor > 65535) divisor = 65535;

	outb(0x43, 0x36);
	outb(0x40, divisor & 0xFF);
	outb(0x40, (divisor >> 8) & 0xFF);

	isr_register(32, timer_heartbeat);
	asm volatile ("sti");
}

void timer_sleep(uint32_t t) {
	size_t start = ticks;
	while ((ticks - start) < t) {
		asm volatile ("hlt");
	}
}

size_t timer_getUptimeMs() {
	if (freq == 0)
		return 0;
	return (ticks * 1000) / freq;
}
