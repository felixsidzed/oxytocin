#pragma once

#define PROCESS_STACK_SIZE 4096

#define PROCESS_STATE_READY			0
#define PROCESS_STATE_RUNNING		1
#define PROCESS_STATE_BLOCKED		2
#define PROCESS_STATE_ZOMBIE		3

typedef uint32_t pid_t;

typedef struct {
	uintptr_t r15, r14, r13, r12, r11, r10, r9, r8;
	uintptr_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
	
	uintptr_t rip, rsp, rflags;
} Context;

typedef struct Process {
	uint32_t id : 24;
	uint32_t state : 2;
	uint32_t flags : 6;

	/// TODO: we could move this to `Context::rdi`
	int ec;
	void* stack;
	const char* name;
	
	Context ctx;

	struct Process* parent;
	struct Process* next;
} Process;

/// @brief Initializes the process management system
void process_init();

/// @brief Creates a new process with a given name and entry point
/// @param name A human-readable name for the process
/// @param entry The function to run as the process entry point
/// @return A pointer to the newly created process
Process* process_create(const char* name, void(*entry)());

/// @brief Terminates the current process
/// @param ec The exit code
/// @note This function does not return
/// @note This function only marks the process for deletion, it will be free'd the next scheduler step
void process_exit(int ec);

/// @brief Retrieves a pointer to the currently running process
/// @return A pointer to the `Process` structure representing the current process
Process* process_getCurrent();

int process_wait(Process* proc);
