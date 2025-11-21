#include "vga.h"

#include "drivers/io.h"

static uint8_t col = 0x07;
static uint16_t oxy_data cx = 0;
static uint16_t oxy_data cy = 0;

static uint16_t* buf = (uint16_t*)0xB8000;

#define vga_putentry(i, ch) buf[(i)] = ((uint8_t)(ch) | col << 8)

void vga_cls() {
	for (uint32_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
		vga_putentry(i, ' ');
	cx = 0, cy = 0;
}

void vga_setColor(uint8_t fg, uint8_t bg) {
	col = (bg << 4) | (fg & 0x0F);
}

void putchar(char ch) {
	switch (ch) {
	case 0:
		return;
	case '\n':
		goto newline;
	case '\t':
		vga_putentry(cy * VGA_WIDTH + cx++, ' ');
		vga_putentry(cy * VGA_WIDTH + cx++, ' ');
		vga_putentry(cy * VGA_WIDTH + cx++, ' ');
		vga_putentry(cy * VGA_WIDTH + cx++, ' ');
		break;
	case '\b':
		vga_putentry(cy * VGA_WIDTH + --cx, ' ');
		break;
	default:
		vga_putentry(cy * VGA_WIDTH + cx, ch);
		cx++;
		break;
	}

	if (cx > VGA_WIDTH) {
	newline:
		cx = 0;
		cy++;

		// TODO
		if (cy > VGA_HEIGHT) {
			vga_cls();
			return;
		}
	}

	uint16_t pos = cy * VGA_WIDTH + cx;
	outb(0x3D4, 0x0E);
	outb(0x3D5, (pos >> 8) & 0xFF);

	outb(0x3D4, 0x0F);
	outb(0x3D5, pos & 0xFF);
}

void puts(const char* s) {
	while (*s)
		putchar(*s++);
}
