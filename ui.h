#pragma once
#include <string> 
#include <vector>
#include <memory>
#include <iostream>
#include "render.h"


class Control;
class Window;

class UiEventListener {
public:
	virtual void onPrepare(Window* window) = 0;
	virtual void onClick(Window* window, Control* sender) = 0;
};

class Control {
public:
	Control()
		: focused_(false)
	{}
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

	virtual bool isFocusable() const
	{
		return false;
	}

	bool focused() const
	{
		return focused_;
	}

	void setFocused(bool value)
	{
		focused_ = value;
	}

protected:
	std::string id_;
	int x_, y_;
	color_t fg_;
	color_t bg_;
	bool focused_;
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
	virtual bool isFocusable() const
	{
		return true;
	}

private:
	color_t bgFocused_;
	int width_;
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
	Window()
		: focusedIndex_(-1)
	{}
	~Window() = default;

	virtual void load(std::istream& is) override;
	virtual void paint() override;
	virtual bool processInput(UiEventListener* listener);
	virtual void prepare(UiEventListener* listener);

	Control* findControl(const std::string& id);

protected:
	int findNextFocusableIndex();

protected:
	int width_;
	int height_;
	int border_type_;
	char border_chars_[6];
	std::vector<std::unique_ptr<Control>> controls_;
	int focusedIndex_;
};

class UiManager final {
public:
	UiManager();

	void showWindow(const std::string& id);
	void closeWindow();

	void showModal(const std::string& id, UiEventListener* listener);
	void closeModal();

	void load(std::istream& is);
	bool preUpdate(float delta);
	void update(float delta);
	void render();
	void shutdown();

private:
	bool isVisible(Window* window);

private:
	std::vector<std::unique_ptr<Window>> windows_;
	std::vector<Window*> visible_;
	Window* activeModal_;
	UiEventListener* activeListener_;
};

extern UiManager g_uiManager;
extern int g_windowWidth;
extern int g_windowHeight;
