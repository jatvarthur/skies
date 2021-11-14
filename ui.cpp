#include "ui.h"
#include <cassert>

void Control::load(std::istream& is)
{
	is >> id_ >> x_ >> y_;
}


void Label::load(std::istream& is)
{
	Control::load(is);

	int stringsNo;
	is >> stringsNo;
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	text_.reserve(stringsNo);
	for (int i = 0; i < stringsNo; ++i) {
		std::string s;
		std::getline(is, s, '\n');
		text_.push_back(std::move(s));
	}
}

void Label::paint()
{
}


void Window::load(std::istream& is)
{
	Control::load(is);

	int controlsNo;
	is >> width_ >> height_ >> controlsNo;
	for (int i = 0; i < controlsNo; ++i) {
		char type;
		std::unique_ptr<Control> c;
		is >> type;
		switch (type) {
		case 'L':
			c = std::make_unique<Label>();
			c->load(is);
			controls_.push_back(std::move(c));
			break;
		default:
			assert(false && "Invalid control type");
		}
	}
}

void Window::paint()
{
}


void UiManager::showWindow(std::string& id)
{

}

void UiManager::closeWindow()
{

}

void UiManager::load(std::istream& is)
{
	int windowsNo;
	is >> windowsNo;
	windows_.reserve(windowsNo);
	for (int i = 0; i < windowsNo; ++i) {
		std::unique_ptr<Window> w = std::make_unique<Window>();
		w->load(is);
		windows_.push_back(std::move(w));
	}
}
