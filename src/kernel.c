#include <string.h>
#include <kprintf.h>
#include <oxystatus.h>

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
	kheap_init(2 * 0x1000);
	
	timer_init(100);
	process_init();
}

void proc1() {
	puts((char*)0xDEADBEEF);
	
	process_exit(67);
}

void kmain() {
	kinit();

	Process* p = process_create("proc1", proc1);
	kprintf("0x%x\n", process_wait(p));

	while (true)
		asm volatile ("hlt");
}

