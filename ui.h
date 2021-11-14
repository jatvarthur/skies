#pragma once
#include <string> 
#include <vector>
#include <memory>
#include <iostream>

class Control {
public:
	Control() = default;
	virtual ~Control() = default;

	virtual void load(std::istream& is);
	virtual void paint() = 0;

protected:
	std::string id_;
	int x_, y_;
};

class Label: public Control
{
public:
	Label() = default;
	~Label() = default;

	virtual void load(std::istream & is) override;
	virtual void paint() override;
private:
	std::vector<std::string> text_;
};

class Window: public Control
{
public:
	Window() = default;
	~Window() = default;

	virtual void load(std::istream& is) override;
	virtual void paint() override;

protected:
	int width_;
	int height_;
	std::vector<std::unique_ptr<Control>> controls_;
};

class UiManager final {
public:
	void showWindow(std::string& id);
	void closeWindow();

	void load(std::istream& is);

private:
	std::vector<std::unique_ptr<Window>> windows_;
	Window* currentWindow_;
};
