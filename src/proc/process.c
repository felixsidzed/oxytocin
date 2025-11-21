#include "process.h"

#include <string.h>

#include "mmu/pmm.h"
#include "mmu/kheap.h"

static oxy_data Process* head = nullptr;
static oxy_data Process* tail = nullptr;
static oxy_data Process* current = nullptr;

void process_init() {
	process_create("sysidl", nullptr);
}

Process* process_create(const char* name, void(*entry)()) {
	Process* proc = kmalloc(sizeof(Process));
	if (!proc)
		return nullptr;

	char* stack = allocpages(PROCESS_STACK_SIZE / 0x1000, PAGE_READWRITE);
	if (!stack) {
		kfree(proc);
		return nullptr;
	}

	if (head) proc->pid = head->pid + 1;
	else proc->pid = 0;

	proc->state = PROCESS_STATE_READY;
	
	int i = 0;
	while (*name && i < 7)
		proc->name[i++] = *name++;

	proc->ctx.cs = 0x08;
	proc->ctx.ss = 0x10;
	proc->stack = (uintptr_t)stack;
	proc->ctx.rip = (uintptr_t)entry;
	proc->ctx.rbp = proc->ctx.rsp = (uintptr_t)stack + PROCESS_STACK_SIZE;
	asm volatile("pushfq\npopq %0" : "=r"(proc->ctx.rflags));

	if (head) {
		tail->next = proc;
		proc->next = head;
		tail = proc;
	} else {
		head = tail = proc;
		proc->next = proc;
	}

	if (!current)
		current = head;

	return proc;
}

// TODO: Improve
oxy_noret process_exit(int ec) {
	if (current->pid)
		current->state = PROCESS_STATE_TERMINATED;

	while (true)
		asm volatile("hlt");
}

void scheduler_step(Context* ctx) {
	if (!current)
		return;

	memcpy(&current->ctx, ctx, sizeof(Context));

	if (current->state == PROCESS_STATE_TERMINATED) {
		Process* next = current->next;

		kfree((void*)current->stack);
		kfree(current);

		current = next;
	} else {
		current->state = PROCESS_STATE_READY;
		current = current->next;
	}

	current->state = PROCESS_STATE_RUNNING;
	memcpy(ctx, &current->ctx, sizeof(Context));
}
