#include "..\render.h"
#include <vector>

SkiiImage::SkiiImage(int width, int height)
	: width_(width)
	, height_(height)
	, buffer_((width > 0 && height > 0) ? new Symbol[width * height] : nullptr)
{
	assert(width >= 0 && height >= 0);
}

SkiiImage::SkiiImage(const SkiiImage& rhs)
	: width_(rhs.width_)
	, height_(rhs.height_)
	, buffer_(rhs.buffer_ ? new Symbol[rhs.width_ * rhs.height_] : nullptr)
{
	std::copy(rhs.buffer_, rhs.buffer_ + rhs.width_ * rhs.height_, buffer_);
}

SkiiImage::~SkiiImage()
{
	delete[] buffer_;
}

void SkiiImage::Swap(SkiiImage& rhs) noexcept
{
	std::swap(width_, rhs.width_);
	std::swap(height_, rhs.height_);
	std::swap(buffer_, rhs.buffer_);
}

const SkiiImage &SkiiImage::operator=(const SkiiImage& rhs)
{
	SkiiImage t(rhs);
	Swap(t);
	return *this;
}

SkiiImage SkiiImage::loadFromStream(std::istream& is)
{
	std::string magic;
	is >> magic;
	if (magic != "skii") {
		return SkiiImage::invalid();
	}
	int version;
	is >> version;
	if (version != 1) {
		return SkiiImage::invalid();
	}

	int w, h, nColors, fg, bg;
	is >> w >> h >> nColors >> fg >> bg;
	SkiiImage result(w, h);
	memset(result.buffer_, 0, w * h * sizeof(*result.buffer_));

	// overrides
	Symbol charOverrides[255] = { 0 };
	for (int i = 0; i < nColors; ++i) {
		int format;
		is >> format;
		if (format == 0 || format == 1) {
			char ch;
			int fg, bg, code = 0;
			is >> ch >> fg >> bg;
			if (format == 1) {
				is >> code;
			}
			charOverrides[ch].ch = code;
			charOverrides[ch].fg = fg;
			charOverrides[ch].bg = bg;
			charOverrides[ch].flags = 1; // has override! 
		} else if (format == 2 || format == 3) {
			int x, y, fg, bg, code = 0;
			is >> x >> y >> fg >> bg;
			if (format == 3) {
				is >> code;
			}
			int o = y * w + x;
			result.buffer_[o].ch = code;
			result.buffer_[o].fg = fg;
			result.buffer_[o].bg = bg;
			result.buffer_[o].flags = 1; // has override! 
		}
	}

	// image map
	Symbol* p = result.buffer_;
	std::vector<char> line(w + 1, '\0');
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	for (int j = 0; j < h; ++j) {
		is.getline(&line[0], (std::streamsize)w + 1, '\n');
		for (int i = 0; i < w; ++i) {
			char ch = line[i];
			int o = j * w + i;
			if (p->flags) {
				if (p->ch == '\0') p->ch = ch;
			} else if (charOverrides[ch].flags) {
				p->ch = charOverrides[ch].ch ? charOverrides[ch].ch : ch;
				p->fg = charOverrides[ch].fg;
				p->bg = charOverrides[ch].bg;
			} else {
				p->ch = line[i];
				p->bg = bg;
				p->fg = fg;
			}

			p->flags = 0;
			p += 1;
		}
	}

	return result;
}

SkiiImage SkiiImage::invalid()
{
	return SkiiImage(0, 0);
}
