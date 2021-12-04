#pragma once
#include "..\ecs.h"
#include "..\ui.h"
#include "Resources.h"
#include "Zeppelin.h"


class TradePostScript : public Script, public UiEventListener {
private:
	struct ResourceData {
		int maxStock;
		int minPrice;
		int maxPrice;
		int rate;
		int stock;
	};

public:
	SCRIPT(TradePostScript)

	TradePostScript(EntityManager& em, Entity_t id)
		: Script(em, id)
	{}

	virtual void awake() override;
	virtual void update(float delta) override;
	virtual void load(std::istream& is) override;
	virtual void onCollision(int x, int y, Entity_t source, Entity_t hit) override;

	virtual void onPrepare(Window* window) override;
	virtual void onClick(Window* window, Control* sender) override;

protected:
	ZeppelinScript* getPlayer();
	int getSellPrice(int name, int stock);
	void sellToPlayer(int name);
	void buyFromPlayer(int name);

	void updateControls(Window* window);


private:
	ResourceData resources_[N_RESOURCES];
	float timeToUpdate_;
};
