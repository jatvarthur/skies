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

	int getCargoResource(int name)
	{
		assert(name >= 0 && name < N_RESOURCES);
		return resources_[name];
	}

private:
	float m_torque_;
	float m_thrust_;
	int resources_[N_RESOURCES];
};

void shdZeppelinLight(const Shader* shader, EntityManager &em);
