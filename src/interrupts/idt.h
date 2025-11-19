#pragma once

typedef struct {
	uint16_t low;
	uint16_t sel;
	uint8_t ist;
	uint8_t attr;
	uint16_t mid;
	uint32_t high;
	uint32_t reserved;
} __attribute__((packed)) IDTEntry;

typedef struct {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed)) IDTR;

#define IDT_INTERRUPT_GATE 0x8E
#define IDT_TRAP_GATE 0x8F
#define IDT_TASK_GATE 0x85

void idt_init();
void idt_setGate(uint8_t num, uint64_t handler, uint8_t flags);
