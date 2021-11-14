#include "..\Zeppelin.h"
#include <cmath>
#include "..\..\keybrd.h"


REGISTER_SCRIPT(ZeppelinScript);
REGISTER_SHADER(shdZeppelinLight);

void ZeppelinScript::update(float delta)
{
	PhysicsComponent *phys = em_.getPhysicsComponent(id_);

	bool isLeft = keyIsDown(KEY_LEFT);
	bool isRight = keyIsDown(KEY_RIGHT);

	if (isLeft && isRight) {
		// do nothing, as steering is cancelled out
		phys->torque = 0;
	} else if (isLeft) {
		phys->torque = -m_torque_;
	} else if (isRight) {
		phys->torque = m_torque_;
	}

	if (keyIsDown(KEY_DOWN)) {
		phys->thrust = -m_thrust_ / 3;
	} else if (keyIsDown(KEY_UP)) {
		phys->thrust = m_thrust_;
	}
}

void ZeppelinScript::load(std::istream& is)
{
	is >> m_thrust_ >> m_torque_;
}


const int FONTH = 8;

void shdZeppelinLight(const Shader* shader, EntityManager& em)
{
	PositionComponent* pos = em.getPositionComponent(shader->entityId);
	float cosTheta = cos(shader->p0 * PI / 180.0f);
	int charRadius = (int)shader->p1;
	float radius = (float)charRadius * FONTH;
	vec2f orient = vec2f(cosf(pos->theta), sinf(pos->theta));

	for (int y = -charRadius; y <= charRadius; ++y) {
		for (int x = -charRadius; x <= charRadius; ++x) {
			vec2f c = vec2f((float)x * FONTH, (float)y * FONTH);
			float len = c.length() - FONTH / 2;
			if (len > radius) continue;
			float dp = dot(orient, c / len);
			if (dp < cosTheta) continue;

			// hilight
			Symbol sym;
			if (getSymbol((int)pos->x + x, (int)pos->y + y, &sym)) {
				//int fg = sym.fg + 8 < N_COLORS ? sym.fg + 8 : sym.fg;
				putAttrs((int)pos->x + x, (int)pos->y + y, COLOR_LIGHT_CYAN, sym.bg);
			}
		}
	}
}
