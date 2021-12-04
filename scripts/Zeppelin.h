#pragma once
#include "..\ecs.h"
#include "Resources.h"


class ZeppelinScript : public Script {
public:
	SCRIPT(ZeppelinScript)

	ZeppelinScript(EntityManager& em, Entity_t id)
		: Script(em, id)
	{}

	virtual void awake() override;
	virtual void update(float delta) override;
	virtual void load(std::istream& is) override;

	int maxCargoWeight()
	{
		return maxCargoWeight_;
	}

	int cargoWeight()
	{
		return cargoWeight_;
	}

	int cargo(int name)
	{
		assert(name >= 0 && name < N_RESOURCES);
		return cargo_[name];
	}

	void buy(int name, int price);
	void sell(int name, int price);

	int money()
	{
		return money_;
	}

	void pay(int value)
	{
		assert(value > 0);
		assert(money_ >= value);
		money_ -= value;
	}

	void earn(int value)
	{
		assert(value > 0);
		money_ += value;
	}

private:
	float torque_;
	float thrust_;
	int maxCargoWeight_;
	
	int cargoWeight_;
	int cargo_[N_RESOURCES];
	int money_;
};

void shdZeppelinLight(const Shader* shader, EntityManager &em);
