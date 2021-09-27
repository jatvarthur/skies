#pragma once
#include <cstdint>

typedef uint8_t color_t;

int putCh(int x, int y, char ch, color_t fg, color_t bg);
int putStr(int x, int y, const char* s, color_t fg, color_t bg);
