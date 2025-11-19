#include "vga/vga.h"
#include "heap/heap.h"
#include "interrupts/isr.h"
#include "interrupts/idt.h"
#include "physmem/physmem.h"

#include "std/mem.h"

void kmain() {
	vga_cls();
	idt_init();
	isr_init();
	asm volatile("sti");
	asm volatile("mov $0, %eax\n div %al");
}
