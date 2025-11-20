#include "mem.h"

void* memset(void *s, int c, size_t count) {
	char* xs = (char*)s;
	size_t len = (-(size_t)s) & (sizeof(size_t)-1);
	size_t cc = c & 0xff;

	if (count > len) {
		count -= len;
		cc |= cc << 8;
		cc |= cc << 16;
		if (sizeof(size_t) == 8)
			cc |= (uint64_t)cc << 32;
		
		for ( ; len > 0; len-- )
			*xs++ = c;
		
		for ( len = count/sizeof(size_t); len > 0; len-- ) {
			*((size_t *)xs) = (size_t)cc;
			xs += sizeof(size_t);
		}
		count &= sizeof(size_t)-1;
	}
	
	for (; count > 0; count--)
		*xs++ = c;
	return s;
}

void* memcpy(void *dst, const void *src, size_t count) {
	char* d = (char*)dst;
	const char* s = (const char*)src;
	int len;

	if (count == 0 || dst == src)
		return dst;

	if (((long)d | (long)s) & (sizeof(long) - 1)) {
		if ((((long)d ^ (long)s) & (sizeof(long) - 1)) || (count < sizeof(long)))
			len = count;
		else
			len = sizeof(long) - ((long)d & (sizeof(long) - 1));
		count -= len;
		for (; len > 0; len--)
			*d++ = *s++;
	}

	for (len = count / sizeof(long); len > 0; len--) {
		*(long*)d = *(long*)s;
		d += sizeof(long);
		s += sizeof(long);
	}

	for (len = count & (sizeof(long) - 1); len > 0; len--)
		*d++ = *s++;

	return dst;
}

bool streq(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(uint8_t*)s1 == *(uint8_t*)s2;
}
