#include "..\TradePost.h"


REGISTER_SCRIPT(TradePostScript);

const float UPDATE_STEP = 60.0;

void TradePostScript::awake()
{
	currentStock_ = 0;
	timeToUpdate_ = 0.0f;
}

void TradePostScript::update(float delta)
{
	timeToUpdate_ += delta;
	if (timeToUpdate_ >= UPDATE_STEP) {
		currentStock_ += m_rate_;
		timeToUpdate_ = 0.0f;
		_debug(L"Trade post #%d, stock=%d\n", id_, currentStock_);
	}
}

void TradePostScript::load(std::istream& is)
{
	is >> m_rate_;
}

void TradePostScript::onCollision(int x, int y, Entity_t source, Entity_t hit)
{

}
