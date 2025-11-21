#include "process.h"

#include <string.h>

#include "mmu/pmm.h"
#include "mmu/kheap.h"

static oxy_data Process* head = nullptr;
static oxy_data Process* tail = nullptr;
static oxy_data Process* current = nullptr;

void process_init() {
	process_create("sysidle", nullptr);
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

	if (head) proc->pid = tail->pid + 1;
	else proc->pid = 0;

	proc->state = PROCESS_STATE_READY;
	
	proc->name = name; // name is always owned
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
	// we cant terminate the current process cuz its running
	if (current->pid) {
		current->ctx.rax = ec;
		current->state = PROCESS_STATE_ZOMBIE;
	}

	// wait for re-schedule
	while (true)
		asm volatile("hlt");
}

Process* process_getCurrent() {
	return current;
}

void scheduler_step(Context* ctx) {
	if (!current)
		return;

	Process* toFree = nullptr;
	Process* next = current->next;

	// reaper
	if (current->state == PROCESS_STATE_ZOMBIE && current->pid) {
		Process* prev = head;
		while (prev->next != current)
			prev = prev->next;
		prev->next = current->next;

		if (current == tail)
			tail = prev;

		toFree = current; // we have to defer ts cuz we are using the process's context rn
		current = next;
	} else {
		memcpy(&current->ctx, ctx, sizeof(Context));
		current->state = PROCESS_STATE_READY;
		current = next;
	}

	current->state = PROCESS_STATE_RUNNING;
	memcpy(ctx, &current->ctx, sizeof(Context));

	if (toFree) {
        freepages((void*)toFree->stack, PROCESS_STACK_SIZE / 0x1000);
        kfree(toFree);
    }
}
