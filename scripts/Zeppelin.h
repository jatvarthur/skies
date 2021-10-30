#pragma once
#include "..\ecs.h"

class ZeppelinScript : public Script {
public:
	ZeppelinScript(EntityManager& em, Entity_t id)
		: Script(em, id)
	{}

	void update(float delta) override;

private:

};

void zeppelinLightShader(const Shader* shader, EntityManager &em);
