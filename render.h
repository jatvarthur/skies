#pragma once
#include <cstdint>

enum Color {
	COLOR_BLACK = 0,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_BLUE,
	COLOR_MAGENTA,
	COLOR_CYAN,
	COLOR_WHITE,
	COLOR_GRAY,
	COLOR_BRIGHT_RED,
	COLOR_BRIGHT_GREEN,
	COLOR_BRIGHT_YELLOW,
	COLOR_BRIGHT_BLUE,
	COLOR_BRIGHT_MAGENTA,
	COLOR_BRIGHT_CYAN,
	COLOR_BRIGHT_WHITE
};

typedef uint8_t color_t;

int drawChar(int x, int y, char ch, color_t fg, color_t bg);
int drawString(int x, int y, const char* s, color_t fg, color_t bg);
