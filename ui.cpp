#include "ui.h"
#include <cassert>
#include <fstream>


void Control::load(std::istream& is)
{
	int fg_color, bg_color;
	is >> id_ >> x_ >> y_ >> fg_color >> bg_color;
	fg_ = fg_color;
	bg_ = bg_color;
}


void Label::load(std::istream& is)
{
	Control::load(is);

	std::getline(is, text_, '\n');
	ltrim(text_);
}

void Label::paint()
{
	drawString(x_, y_, text_.c_str(), fg_, bg_);
}


void NumberField::load(std::istream& is)
{
	Control::load(is);

	int value;
	is >> width_ >> value;
	setValue(value);
}

void NumberField::paint()
{
	int ofs = std::max(0, width_ - (int)text_.size() + 1) / 2;
	drawString(x_ + ofs, y_, text_.c_str(), fg_, bg_);
}

void NumberField::setValue(int value)
{
	value_ = value;
	text_ = std::to_string(value);
}


void Text::load(std::istream& is)
{
	Control::load(is);

	int nStrings;
	is >> nStrings;
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	text_.reserve(nStrings);
	for (int i = 0; i < nStrings; ++i) {
		std::string s;
		std::getline(is, s, '\n');
		text_.push_back(std::move(s));
	}
}

void Text::paint()
{
	for (int i = 0; i < (int)text_.size(); ++i) {
		drawString(x_, y_ + i, text_[i].c_str(), fg_, bg_);
	}
}

void Button::load(std::istream& is)
{
	Control::load(is);

	std::getline(is, caption_, '\n');
	ltrim(caption_);
}

void Button::paint()
{
	drawString(x_, y_, caption_.c_str(), fg_, bg_);
}


void Image::load(std::istream& is)
{
	Control::load(is);

	std::string fileName;
	is >> fileName;

	std::ifstream imageFile(getTextAssetFileName(fileName));
	image_ = SkiiImage::loadFromStream(imageFile);
}

void Image::paint()
{
	drawImage(x_, y_, 0, 0, image_.width(), image_.height(), image_);
}

std::unique_ptr<Control> make_control(char type)
{
	switch (type) {
	case 'L': return std::make_unique<Label>();
	case 'N': return std::make_unique<NumberField>();
	case 'T': return std::make_unique<Text>();
	case 'B': return std::make_unique<Button>();
	case 'I': return std::make_unique<Image>();
	default:  assert(false && "Invalid control type");
	}
	return std::unique_ptr<Control>();
}

void Window::load(std::istream& is)
{
	Control::load(is);

	int nControls;
	is >> width_ >> height_ >> border_type_;
	if (border_type_ == 0) {
		std::fill_n(border_chars_, NELEMS(border_chars_), ' ');
	} else if (border_type_ == 1) {
		for (int i = 0; i < NELEMS(border_chars_); ++i) {
			is >> border_chars_[i];
		}
	} else if (border_type_ == 2) {
		for (int i = 0; i < NELEMS(border_chars_); ++i) {
			int c;
			is >> c;
			border_chars_[i] = c;
		}
	}

	is >> nControls;
	for (int i = 0; i < nControls; ++i) {
		char type;
		is >> type;
		std::unique_ptr<Control> c = make_control(type);
		c->load(is);
		controls_.push_back(std::move(c));
	}
}

const int CHAR_HORZ = 0;
const int CHAR_VERT = 1;
const int CHAR_LT = 2;
const int CHAR_LB = 3;
const int CHAR_RT = 4;
const int CHAR_RB = 5;

void Window::paint()
{
	drawLine(x_, y_, width_,
		border_chars_[CHAR_LT], border_chars_[CHAR_HORZ], border_chars_[CHAR_RT],
		fg_, bg_);

	int yh = y_ + height_ - 1;
	for (int y = y_ + 1 ; y < yh; ++y) {
		drawLine(x_, y, width_,
			border_chars_[CHAR_VERT], ' ', border_chars_[CHAR_VERT],
			fg_, bg_);
	}

	drawLine(x_, y_ + height_ - 1, width_,
		border_chars_[CHAR_LB], border_chars_[CHAR_HORZ], border_chars_[CHAR_RB],
		fg_, bg_);

	// todo: transformation stack
	drawTranslate(x_ + 1, y_ + 1);
	for (std::unique_ptr<Control>& c : controls_) {
		c->paint();
	}
}


void UiManager::showWindow(const std::string& id)
{
	for (std::unique_ptr<Window>& w : windows_) {
		if (w->is(id)) {
			if (!isVisible(w.get())) {
				visible_.push_back(w.get());
			}
			break;
		}
	}
}

void UiManager::closeWindow()
{

}

bool UiManager::isVisible(Window* window)
{
	for (Window* w : visible_) {
		if (w == window) return true;
	}
	return false;
}

void UiManager::load(std::istream& is)
{
	int nWindows;
	is >> nWindows;
	windows_.reserve(nWindows);
	for (int i = 0; i < nWindows; ++i) {
		std::unique_ptr<Window> w = std::make_unique<Window>();
		w->load(is);
		windows_.push_back(std::move(w));
	}
}

void UiManager::render()
{
	for (Window* w : visible_) {
		drawIdentity();
		w->paint();
	}
}

void UiManager::shutdown()
{
	visible_.clear();
	windows_.clear();
}

