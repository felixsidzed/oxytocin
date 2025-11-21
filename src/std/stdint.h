#pragma once

#include <stdint-gcc.h>

#if defined(__x86_64__) || defined(_M_X64)
	typedef long long ssize_t;
	typedef unsigned long long size_t;
#else
	typedef int ssize_t;
	typedef unsigned int size_t;
#endif

#define true 1
#define false 0
#define bool char

typedef void* nullptr_t;
#define nullptr ((nullptr_t)0)
