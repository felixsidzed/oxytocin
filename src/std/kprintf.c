#include <kprintf.h>

#include "vga/vga.h"

#define PRINTF_STATE_NORMAL 0
#define PRINTF_STATE_LENGTH 1
#define PRINTF_STATE_LENGTH_SHORT 2
#define PRINTF_STATE_LENGTH_LONG 3
#define PRINTF_STATE_SPEC 4

#define PRINTF_LENGTH_DEFAULT 0
#define PRINTF_LENGTH_SHORT_SHORT 1
#define PRINTF_LENGTH_SHORT 2
#define PRINTF_LENGTH_LONG 3
#define PRINTF_LENGTH_LONG_LONG 4

static const char hexChars[] = "0123456789abcdef";

static void printunsigned(unsigned long long number, int radix) {
	int pos = 0;
	char buffer[32];

	do {
		unsigned long long rem = number % radix;
		number /= radix;
		buffer[pos++] = hexChars[rem];
	} while (number > 0);

	while (--pos >= 0)
		putchar(buffer[pos]);
}

static void printsigned(long long number, int radix) {
	if (number < 0) {
		putchar('-');
		printunsigned(-number, radix);
	} else
		printunsigned(number, radix);
}

void kprintf(const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);

	kvprintf(fmt, va);
	
	va_end(va);
}

void kvprintf(const char* fmt, va_list va) {
	int radix = 10;
	bool sign = false;
	bool number = false;
	int state = PRINTF_STATE_NORMAL;
	int length = PRINTF_LENGTH_DEFAULT;

	while (*fmt) {
		switch (state) {
		case PRINTF_STATE_NORMAL:
			switch (*fmt) {
			case '%':
				state = PRINTF_STATE_LENGTH;
				break;
			default:
				putchar(*fmt);
				break;
			}
			break;

		case PRINTF_STATE_LENGTH:
			switch (*fmt) {
			case 'h':
				length = PRINTF_LENGTH_SHORT;
				state = PRINTF_STATE_LENGTH_SHORT;
				break;
			case 'l':
				length = PRINTF_LENGTH_LONG;
				state = PRINTF_STATE_LENGTH_LONG;
				break;
			default:
				goto PRINTF_STATE_SPEC_;
			}
			break;

		case PRINTF_STATE_LENGTH_SHORT:
			if (*fmt == 'h') {
				length = PRINTF_LENGTH_SHORT_SHORT;
				state = PRINTF_STATE_SPEC;
			} else
				goto PRINTF_STATE_SPEC_;
			break;

		case PRINTF_STATE_LENGTH_LONG:
			if (*fmt == 'l') {
				state = PRINTF_STATE_SPEC;
				length = PRINTF_LENGTH_LONG_LONG;
			} else
				goto PRINTF_STATE_SPEC_;
			break;

		case PRINTF_STATE_SPEC:
		PRINTF_STATE_SPEC_:
			switch (*fmt) {
			case 'c':
				putchar((char)va_arg(va, int));
				break;

			case 's':
				puts((const char *)va_arg(va, const char *));
				break;

			case '%':
				putchar('\n');
				break;

			case 'd':
			case 'i':
				radix = 10;
				sign = true;
				number = true;
				break;

			case 'u':
				radix = 10;
				sign = false;
				number = true;
				break;

			case 'X':
			case 'x':
			case 'p':
				radix = 16;
				sign = false;
				number = true;
				break;

			case 'o':
				radix = 8;
				sign = false;
				number = true;
				break;

			default:
				break;
			}

			if (number) {
				if (sign) {
					switch (length) {
					case PRINTF_LENGTH_SHORT_SHORT:
					case PRINTF_LENGTH_SHORT:
					case PRINTF_LENGTH_DEFAULT:
						printsigned(va_arg(va, int), radix);
						break;

					case PRINTF_LENGTH_LONG:
						printsigned(va_arg(va, long), radix);
						break;

					case PRINTF_LENGTH_LONG_LONG:
						printsigned(va_arg(va, long long), radix);
						break;
					}
				} else {
					switch (length) {
					case PRINTF_LENGTH_SHORT_SHORT:
					case PRINTF_LENGTH_SHORT:
					case PRINTF_LENGTH_DEFAULT:
						printsigned(va_arg(va, unsigned int), radix);
						break;

					case PRINTF_LENGTH_LONG:
						printsigned(va_arg(va, unsigned long), radix);
						break;

					case PRINTF_LENGTH_LONG_LONG:
						printsigned(va_arg(va, unsigned long long), radix);
						break;
					}
				}
			}

			state = PRINTF_STATE_NORMAL;
			length = PRINTF_LENGTH_DEFAULT;
			radix = 10;
			sign = false;
			number = false;
			break;
		}

		fmt++;
	}
}