#pragma once
#include "..\ecs.h"

class ZeppelinScript : public Script {
public:
	ZeppelinScript(EntityManager& em, Entity_t id)
		: Script(em, id)
	{}

	void update(float delta) override;
	void load(std::istream& is) override;


private:
	float m_torque_;
	float m_thrust_;
};

void shdZeppelinLight(const Shader* shader, EntityManager &em);
