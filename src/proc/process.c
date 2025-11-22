#include "process.h"

#include <string.h>

#include "mmu/pmm.h"
#include "mmu/kheap.h"

#include "interrupts/isr.h"

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

	char* stack = nullptr;
	if (head) {
	#if PROCESS_STACK_SIZE == 0x1000
		stack = allocpage(PAGE_READWRITE);
	#else
		stack = allocpages(PROCESS_STACK_SIZE / 0x1000, PAGE_READWRITE);
	#endif
		if (!stack) {
			kfree(proc);
			return nullptr;
		}
	}

	if (tail) proc->id = tail->id + 1;
	else proc->id = 0;

	proc->ec = 0;
	proc->name = name;
	proc->state = PROCESS_STATE_READY;

	if (stack) {
		proc->stack = stack;
		proc->ctx.rsp = (uintptr_t)stack + PROCESS_STACK_SIZE;
	}
	proc->ctx.rip = (uintptr_t)entry;
	asm volatile("pushfq\npop %0" : "=r"(proc->ctx.rflags));

	if (!head) {
		head = tail = proc;
		proc->next = proc;
	} else {
		tail->next = proc;
		tail = proc;
		proc->next = head;
	}

	if (!current)
		current = proc;

	return proc;
}

oxy_noret process_exit(int ec) {
	if (current->id) {
		current->ec = ec;
		current->state = PROCESS_STATE_ZOMBIE;

		Process* prev = head;
		while (prev->next != current)
			prev = prev->next;
		prev->next = current->next;
		if (current == tail)
			tail = prev;
	}

	// TODO: better rescheduling method
	while (true)
		asm volatile("hlt");
}

Process* process_getCurrent() {
	return current;
}

int process_wait(Process* proc) {
	// TODO: better rescheduling method
	while (proc->state != PROCESS_STATE_ZOMBIE)
		asm volatile("hlt");
	
	int ec = proc->ec;

	kfree(proc);
	if (current->stack) {
#if PROCESS_STACK_SIZE == 0x1000
		freepage(current->stack);
#else
		freepages(current->stack, PROCESS_STACK_SIZE / 0x1000);
#endif
	}

	return ec;
}

void scheduler_step(ISRContext* ctx) {
	if (!current)
		return;

	Context* pctx = &current->ctx;
	memcpy(pctx, ctx, sizeof(uintptr_t)*15);
	pctx->rip = ctx->rip; pctx->rsp = ctx->rsp; pctx->rflags = ctx->rflags;

	if (current->state != PROCESS_STATE_ZOMBIE)
		current->state = PROCESS_STATE_READY;
	do {
		current = current->next;
	} while (current->state != PROCESS_STATE_READY);
	current->state = PROCESS_STATE_RUNNING;

	pctx = &current->ctx;
	memcpy(ctx, pctx, sizeof(uintptr_t)*15);
	ctx->rip = pctx->rip; ctx->rsp = pctx->rsp; ctx->rflags = pctx->rflags;
}
