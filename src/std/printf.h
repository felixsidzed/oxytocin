#pragma once

#include "stdarg.h"

/// @brief Prints a formatted string to the kernel console
/// @param fmt The format string
/// @param ... Additional arguments for formatting
void kprintf(const char* fmt, ...);

/// @brief Prints a formatted string to the kernel console
/// @param fmt The format string
/// @param va Additional arguments for formatting
void kvprintf(const char* fmt, va_list va);
