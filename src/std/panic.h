#pragma once

#include "interrupts/isr.h"

/// @brief Halts the system and displays a panic message
/// @param message The formatted message to display
/// @note This function does not return
/// During a panic, the system provides an interactive debug console
/// with the following commands:
/// - reboot       : Restarts the system immediately
/// - frame        : Displays the current RIP, RSP, interrupt number, and error code
/// - cls          : Clears the panic screen
/// - regs         : Dumps all general-purpose and some control registers
/// - traceback    : Shows the current call stack
/// - stack        : Displays the raw stack memory
void kpanic(const char* message, ...);

/// @brief Halts the system and displays a panic message
/// @param ctx The captured CPU state
/// @param message The formatted message to display
/// @note This function does not return
/// During a panic, the system provides an interactive debug console
/// with the following commands:
/// - reboot       : Restarts the system immediately
/// - frame        : Displays the current CPU state: RIP, RSP, interrupt number, and error code
/// - cls          : Clears the panic/debug screen
/// - regs         : Dumps all general-purpose and some control registers
/// - traceback    : Shows the current call stack
/// - stack        : Displays the raw stack memory
void kpanic_ctx(ISRContext* ctx, const char* message, ...);
