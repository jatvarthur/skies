#pragma once
#include <string> 
#include <vector>
#include <memory>
#include <iostream>
#include "render.h"

class Control {
public:
	Control() = default;
	virtual ~Control() = default;

	virtual void load(std::istream& is);
	virtual void paint() = 0;

	std::string id() const
	{ 
		return id_; 
	}

	bool is(const std::string& id) const 
	{ 
		return id == id_; 
	}

protected:
	std::string id_;
	int x_, y_;
	color_t fg_;
	color_t bg_;
};

class Label : public Control
{
public:
	Label() = default;
	~Label() = default;

	virtual void load(std::istream & is) override;
	virtual void paint() override;
private:
	std::string text_;
};

class NumberField : public Control
{
public:
	NumberField() = default;
	~NumberField() = default;

	virtual void load(std::istream& is) override;
	virtual void paint() override;

	void setValue(int value);

private:
	int value_;
	int width_;
	mutable std::string text_;
};

class Text : public Control
{
public:
	Text() = default;
	~Text() = default;

	virtual void load(std::istream& is) override;
	virtual void paint() override;
private:
	std::vector<std::string> text_;
};

class Button : public Control
{
public:
	Button() = default;
	~Button() = default;

	virtual void load(std::istream& is) override;
	virtual void paint() override;
private:
	std::string caption_;
};

class Image : public Control
{
public:
	Image()
		: image_(SkiiImage::invalid())
	{}

	~Image() = default;

	virtual void load(std::istream & is) override;
	virtual void paint() override;
private:
	SkiiImage image_;
};

class Window : public Control
{
public:
	Window() = default;
	~Window() = default;

	virtual void load(std::istream& is) override;
	virtual void paint() override;

protected:
	int width_;
	int height_;
	int border_type_;
	char border_chars_[6];
	std::vector<std::unique_ptr<Control>> controls_;
};

class UiManager final {
public:
	void showWindow(const std::string& id);
	void closeWindow();


	void load(std::istream& is);
	void render();
	void shutdown();

private:
	bool isVisible(Window* window);

private:
	std::vector<std::unique_ptr<Window>> windows_;
	std::vector<Window*> visible_;
};
