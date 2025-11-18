#pragma once

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void vga_cls();
void vga_setColor(uint8_t fg, uint8_t bg);

void putchar(char ch);
void puts(const char* s);
