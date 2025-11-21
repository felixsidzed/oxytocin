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
	int* page = allocpage(PAGE_READONLY);
	// this will cause a page fault
	*(int*)page = 13879; // '67'
	process_exit(67);
}

void kmain() {
	kinit();

	Process* proc = process_create("proc1", proc1);
	// TODO: Proper way of waiting for a process to finish
	timer_sleep(16);
	
	// see /src/std/oxystatus.h
	if (process_getExitCode(proc) == STATUS_ACCESS_VIOLATION) {
		puts("proc1 caused an access violation\n");
	} else
		puts("proc1 exited normally\n");

	while (true)
		asm volatile ("hlt");
}

