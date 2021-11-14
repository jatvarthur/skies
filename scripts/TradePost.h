#pragma once
#include "..\ecs.h"

class TradePostScript : public Script {
public:
	TradePostScript(EntityManager& em, Entity_t id)
		: Script(em, id)
	{}

	virtual void awake() override;
	virtual void update(float delta) override;
	virtual void load(std::istream& is) override;
	virtual void onCollision(int x, int y, Entity_t source, Entity_t hit) override;

private:
	int m_rate_;
	int currentStock_;
	float timeToUpdate_;
};
