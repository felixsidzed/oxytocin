#include "std/mem.h"

#include "vga/vga.h"

#include "heap/heap.h"
#include "heap/kheap.h"

#include "interrupts/isr.h"
#include "interrupts/idt.h"

#include "physmem/physmem.h"

#include "drivers/timer.h"

void kinit() {
	vga_cls();
	idt_init();
	isr_init();
	pmm_init();
	kheap_init(8 * 0x1000);
	
	timer_init(100);
}

void kmain() {
    kinit();

	char* a = allocpage(PAGE_PROT_WRITE);
	*a = 'A';
	pmm_protect(a, PAGE_PROT_READ);
	//*a = 'B'; // will page fault
	putchar(*a);

	char* b = kmalloc(1);
	*b = 0xC3;

	//pmm_protect(b, PAGE_PROT_EXECUTE);
	//((void(*)())b)(); // will page fault unless the line above is uncommented

    while (true)
        asm volatile ("hlt");
}

