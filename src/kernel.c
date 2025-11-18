#include "vga/vga.h"
#include "heap/heap.h"
#include "interrupts/isr.h"
#include "interrupts/idt.h"
#include "physmem/physmem.h"

#include "std/mem.h"

void kmain() {
	vga_cls();
	idt_init();
	// TODO: Get the number of pages based on the E820 map
	pmm_init(16 * 0x1000);

	Heap* heap = heap_create(4096);
	char* p = heap_alloc(heap, 15);
	memcpy(p, "Hello, World!", 14);
	puts(p);
}
