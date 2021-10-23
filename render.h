#pragma once
#include <cstdint>
#include <cassert>
#include <iostream>

enum Color {
	// dark shades
	COLOR_BLACK = 0,				// 0
	COLOR_DARK_RED,
	COLOR_DARK_GREEN,
	COLOR_DARK_YELLOW,
	COLOR_DARK_BLUE,				// 4
	COLOR_DARK_MAGENTA,
	COLOR_DARK_CYAN,
	COLOR_DARK_WHITE,
	// normal shades
	COLOR_DARK_GRAY,				// 8
	COLOR_RED,
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_BLUE,						// 12
	COLOR_MAGENTA,
	COLOR_CYAN,
	COLOR_WHITE,
	// light shades
	COLOR_LIGHT_GRAY,				// 16
	COLOR_LIGHT_RED,
	COLOR_LIGHT_GREEN,
	COLOR_LIGHT_YELLOW,
	COLOR_LIGHT_BLUE,				// 20
	COLOR_LIGHT_MAGENTA,
	COLOR_LIGHT_CYAN,
	COLOR_LIGHT_WHITE,
	// bright shades
	COLOR_BRIGHT_GRAY,				// 24
	COLOR_BRIGHT_RED,
	COLOR_BRIGHT_GREEN,
	COLOR_BRIGHT_YELLOW,
	COLOR_BRIGHT_BLUE,				// 28
	COLOR_BRIGHT_MAGENTA,
	COLOR_BRIGHT_CYAN,
	COLOR_BRIGHT_WHITE				// 31
};

typedef uint8_t color_t;

typedef char schar_t;

struct Symbol {
	schar_t ch;
	color_t fg;
	color_t bg;
	uint8_t flags; // reserved, must be 0
};

class SkiiImage final {
public:
	SkiiImage(int width, int height);
	SkiiImage(const SkiiImage& rhs);
	~SkiiImage();

	const SkiiImage &operator=(const SkiiImage& rhs);

	int width() const 
	{ 
		return width_;
	}

	int height() const
	{
		return height_;
	}

	Symbol at(int x, int y) const
	{
		assert(x >= 0 && x < width_);
		assert(y >= 0 && y < height_);
		return buffer_[y * width_ + x];
	}

	bool isValid() const
	{
		return buffer_ != nullptr;
	}

	operator bool() const
	{
		return isValid();
	}

public:
	static SkiiImage loadFromStream(std::istream& is);
	static SkiiImage invalid();

private:
	void Swap(SkiiImage& rhs) noexcept;

private:
	int width_;
	int height_;
	Symbol* buffer_;
};


int drawChar(int x, int y, char ch, color_t fg, color_t bg);
int drawString(int x, int y, const char* s, color_t fg, color_t bg);
int drawImage(int xDst, int yDst, int xSrx, int ySrc, int wSrc, int hSrc, const SkiiImage& image);
