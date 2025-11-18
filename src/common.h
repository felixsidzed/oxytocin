#pragma once

#define oxy_noret __attribute__((noreturn)) void
#define oxy_nobss __attribute__((section(".data")))
