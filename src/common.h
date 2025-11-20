#pragma once

#define oxy_noret __attribute__((noreturn)) void
#define oxy_data __attribute__((section(".data")))
#define oxy_unreachable __builtin_unreachable
