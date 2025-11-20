#pragma once

#include "interrupts/isr.h"

/// @brief Initializes the system timer with a given frequency
/// @param freq Frequency of the timer
void timer_init(uint32_t freq);

/// @brief Pauses execution for a specified number of timer ticks
/// @param ticks Number of timer ticks to sleep
void timer_sleep(uint32_t ticks);

/// @brief Returns the uptime of the system in milliseconds
/// @return Number of milliseconds since system start
size_t timer_getUptimeMs();
