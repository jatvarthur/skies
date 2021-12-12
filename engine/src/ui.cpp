#include "..\ui.h"
#include <cassert>
#include <fstream>
#include "..\keybrd.h"
#include "..\engine.h"

void Control::load(std::istream& is)
{
	int fg, bg;
	is >> id_ >> x_ >> y_ >> fg >> bg;
	fg_ = fg;
	bg_ = bg;
}


void Label::load(std::istream& is)
{
	Control::load(is);

	loadString(is, text_, false);
}

void Label::paint()
{
	drawString(x_, y_, text_.c_str(), fg_, bg_);
}

void Label::setText(const std::string& value)
{
	text_ = value;
}


const int NF_ALIGN_LEFT = 0;
const int NF_ALIGN_CENTER = 1;
const int NF_ALIGN_RIGHT = 2;

void NumberField::load(std::istream& is)
{
	Control::load(is);
	
	int currencyCharCode;
	is >> width_ >> align_ >> currencyCharCode >> value_;
	currencyChar_ = currencyCharCode;

	updateText();
}

void NumberField::paint()
{
	int ofs = 0;
	if (align_ == NF_ALIGN_CENTER) {
		ofs = std::max(0, width_ - (int)text_.size() + 1) / 2;
	} else if (align_ == NF_ALIGN_RIGHT) {
		ofs = std::max(0, width_ - (int)text_.size());
	}
	drawString(x_ + ofs, y_, text_.c_str(), fg_, bg_);
}

void NumberField::setValue(int value)
{
	if (value_ == value) return;
	value_ = value;
	updateText();
}

void NumberField::updateText()
{
	text_ = std::to_string(value_);
	if (currencyChar_) text_.insert(0, 1, currencyChar_);
}


void Text::load(std::istream& is)
{
	Control::load(is);

	loadText(is, text_);
}

void Text::paint()
{
	for (int i = 0; i < (int)text_.size(); ++i) {
		drawString(x_, y_ + i, text_[i].c_str(), fg_, bg_);
	}
}

void Text::setText(const std::vector<std::string>& value)
{
	text_.assign(value.cbegin(), value.cend());
}


void Button::load(std::istream& is)
{
	Control::load(is);

	int bgFocused, fgDisabled;
	is >> bgFocused >> fgDisabled >> row_ >> col_ >> width_;
	bgFocused_ = bgFocused;
	fgDisabled_ = fgDisabled;

	std::getline(is, caption_, '\n');
	ltrim(caption_);

	int spacesTotal = width_ - caption_.size();
	assert(spacesTotal >= 0);
	int spacesLeft = spacesTotal / 2;
	if (spacesLeft > 0) {
		caption_.insert(caption_.begin(), spacesLeft, ' ');
	}
	if (spacesTotal - spacesLeft > 0) {
		caption_.insert(caption_.end(), spacesTotal - spacesLeft, ' ');
	}
}

void Button::paint()
{
	color_t fg = enabled() ? fg_ : fgDisabled_;
	color_t bg = focused() ? bgFocused_ : bg_;

	drawString(x_, y_, caption_.c_str(), fg, bg);
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
	is >> width_ >> height_ >> borderType_;
	if (borderType_ == 0) {
		std::fill_n(borderChars_, NELEMS(borderChars_), ' ');
	} else if (borderType_ == 1) {
		for (int i = 0; i < NELEMS(borderChars_); ++i) {
			is >> borderChars_[i];
		}
	} else if (borderType_ == 2) {
		for (int i = 0; i < NELEMS(borderChars_); ++i) {
			int c;
			is >> c;
			borderChars_[i] = c;
		}
	}

	is >> nControls;
	for (int i = 0; i < nControls; ++i) {
		char type;
		is >> type;
		std::unique_ptr<Control> c = make_control(type);
		c->load(is);
		if (c->row() > maxRow_) maxRow_ = c->row();
		if (c->col() > maxCol_) maxCol_ = c->col();
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
		borderChars_[CHAR_LT], borderChars_[CHAR_HORZ], borderChars_[CHAR_RT],
		fg_, bg_);

	int yh = y_ + height_ - 1;
	for (int y = y_ + 1 ; y < yh; ++y) {
		drawLine(x_, y, width_,
			borderChars_[CHAR_VERT], ' ', borderChars_[CHAR_VERT],
			fg_, bg_);
	}

	drawLine(x_, y_ + height_ - 1, width_,
		borderChars_[CHAR_LB], borderChars_[CHAR_HORZ], borderChars_[CHAR_RB],
		fg_, bg_);

	// todo: transformation stack
	int d = borderType_ == 0 ? 0 : 1;
	drawTranslate(x_ + d, y_ + d);
	for (std::unique_ptr<Control>& c : controls_) {
		c->paint();
	}
}

bool Window::processInput()
{
	if (keyIsPressed(KEY_TAB)) {
		focusNextControl();
		return true;
	}

	bool isLeft = keyIsPressed(KEY_LEFT);
	bool isRight = keyIsPressed(KEY_RIGHT);
	bool isUp = keyIsPressed(KEY_UP);
	bool isDown = keyIsPressed(KEY_DOWN);
	int focusedIndex = -1;
	if (isLeft && focusedRow_ != -1 && focusedCol_ > 0) {
		focusedIndex = findGridFocusableIndex(focusedRow_, focusedCol_, 0, -1);
	}

	if (isRight && focusedRow_ != -1 && focusedCol_ < maxCol_) {
		focusedIndex = findGridFocusableIndex(focusedRow_, focusedCol_, 0, 1);
	}
	
	if (isUp && focusedCol_ != -1 && focusedRow_ > 0) {
		focusedIndex = findGridFocusableIndex(focusedRow_, focusedCol_, -1, 0);
	}

	if (isDown && focusedCol_ != -1 && focusedRow_ < maxRow_) {
		focusedIndex = findGridFocusableIndex(focusedRow_, focusedCol_, 1, 0);
	}

	if (focusedIndex != -1) {
		focusControl(focusedIndex);
		return true;
	}

	bool isEnter = keyIsPressed(KEY_RETURN);
	bool isSpace = keyIsPressed(KEY_SPACE);
	if ((isEnter || isSpace) && listener_ && focusedIndex_ != -1 
			&& controls_[focusedIndex_]->enabled()) {
		listener_->onClick(this, controls_[focusedIndex_].get());
		return true;
	}

	return false;
}

void Window::prepare()
{
	if (focusedIndex_ != -1) controls_[focusedIndex_]->setFocused(false);
	focusedIndex_ = -1;
	focusNextControl();

	if (listener_ != nullptr) listener_->onPrepare(this);
}

void Window::attachListener(UiEventListener* listener)
{
	listener_ = listener;
}

void Window::detachListener()
{
	listener_ = nullptr;
}

Control* Window::findControl(const std::string& id)
{
	for (std::unique_ptr<Control>& c : controls_) {
		if (c->is(id)) {
			return c.get();
		}
	}
	assert(false && "Unknown control id");
	return nullptr;
}

void Window::enableControl(Control* control, bool enabled)
{
	control->setEnabled(enabled);
}

void Window::focusControl(int focusedIndex)
{
	if (focusedIndex_ != -1) controls_[focusedIndex_]->setFocused(false);
	focusedIndex_ = focusedIndex;
	if (focusedIndex_ != -1) {
		controls_[focusedIndex_]->setFocused(true);
		focusedRow_ = controls_[focusedIndex_]->row();
		focusedCol_ = controls_[focusedIndex_]->col();
	} else {
		focusedRow_ = -1;
		focusedCol_ = -1;
	}
}


int Window::findNextFocusableIndex()
{
	if (controls_.empty()) {
		return -1;
	}

	int nextIndex = (focusedIndex_ + 1) % controls_.size();
	do {
		if (controls_[nextIndex]->focusable()) {
			return nextIndex;
		}
		nextIndex = (nextIndex + 1) % controls_.size();
	} while (nextIndex - 1 != focusedIndex_);

	return -1;
}

void Window::focusNextControl()
{
	int nextIndex = findNextFocusableIndex();
	focusControl(nextIndex);
}

int Window::findGridFocusableIndex(int row, int col, int deltaRow, int deltaCol)
{
	if (maxRow_ == -1 || maxCol_ == -1) return -1;

	row += deltaRow;
	col += deltaCol;
	while (row >= 0 && col >= 0 && row <= maxRow_ && col <= maxCol_) {
		for (int i = 0; i < (int)controls_.size(); ++i) {
			Control* c = controls_[i].get();
			if (c->row() == row && c->col() == col) {
				if (!c->focusable()) {
					break;
				}
				return i;
			}
		}
		row += deltaRow;
		col += deltaCol;
	}
	return -1;
}


UiManager::UiManager()
	: activeModal_(nullptr)
{
}

Window* UiManager::showWindow(const std::string& id, UiEventListener* listener)
{
	Window* window = findWindow(id);
	if (!window) return nullptr;

	if (isVisible(window)) return window;
	visible_.push_back(window);
	window->attachListener(listener);
	window->prepare();
	return window;
}

void UiManager::closeWindow(const std::string& id)
{
	Window* window = findWindow(id);
	if (!window) return;
	
	window->detachListener();
	visible_.erase(std::remove_if(visible_.begin(), visible_.end(),
		[window](const Window* item) { return item == window; }
	));
}

Window* UiManager::findWindow(const std::string& id)
{
	for (std::unique_ptr<Window>& w : windows_) {
		if (w->is(id)) {
			return w.get();
		}
	}
	assert(false && "Unknown window identifier");
	return nullptr;
}

void UiManager::showModal(const std::string& id, UiEventListener* listener)
{
	assert(activeModal_ == nullptr);

	Window* window = findWindow(id);
	if (!window) return;

	assert(!isVisible(window));
	if (isVisible(window)) return;

	activeModal_ = window;
	activeModal_->attachListener(listener);
	activeModal_->setPosition(
		(g_windowWidth - window->width()) / 2,
		(g_windowHeight - window->height()) / 2
	);
	activeModal_->prepare();
}

void UiManager::closeModal()
{
	assert(activeModal_ != nullptr);
	activeModal_ = nullptr;
}

bool UiManager::isVisible(Window* window)
{
	for (Window* w : visible_) {
		if (w == window) return true;
	}
	return activeModal_ == window;
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

bool UiManager::preUpdate(float delta)
{
	if (!activeModal_) {
		return true;
	}

	if (keyIsPressed(KEY_ESCAPE)) {
		closeModal();
		return false;
	}

	activeModal_->processInput();
	return false;
}

void UiManager::update(float delta)
{
}

void UiManager::render()
{
	for (Window* w : visible_) {
		drawIdentity();
		w->paint();
	}

	if (activeModal_) {
		drawIdentity();
		activeModal_->paint();
	}
}

void UiManager::shutdown()
{
	if (activeModal_) closeModal();
	visible_.clear();
	windows_.clear();
}

