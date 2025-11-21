#include <string.h>

#include "vga/vga.h"

#include "mmu/pmm.h"
#include "mmu/heap.h"
#include "mmu/kheap.h"

#include "interrupts/isr.h"
#include "interrupts/idt.h"

#include "drivers/timer.h"

#include "proc/process.h"

void kinit() {
	vga_cls();
	idt_init();
	isr_init();
	pmm_init();
	kheap_init(8 * 0x1000);
	
	timer_init(100);
	process_init();
}

void proc1() {
	puts("Hello, World!\n");
	process_exit(0);
}

void proc2() {
	puts("Hello, EVIL World!\n");
	process_exit(0);
}

void kmain() {
	kinit();

	process_create("proc1", proc1);
	timer_sleep(32);
	process_create("proc2", proc2);

	while (true)
		asm volatile ("hlt");
}

