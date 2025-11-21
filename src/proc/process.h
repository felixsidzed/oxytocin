#pragma once

#include "interrupts/isr.h"

#define PROCESS_STACK_SIZE 4096

#define PROCESS_STATE_READY			0
#define PROCESS_STATE_RUNNING		1
#define PROCESS_STATE_BLOCKED		2
#define PROCESS_STATE_TERMINATED	3

/// TODO: Multi-threading
typedef struct Process {
	uint32_t pid;

	uint8_t state;
	
	char name[8];

	Context ctx;
	uintptr_t stack;

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
