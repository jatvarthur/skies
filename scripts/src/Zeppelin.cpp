#include "..\Zeppelin.h"
#include <cmath>
#include "..\..\keybrd.h"


REGISTER_SCRIPT(ZeppelinScript);
REGISTER_SHADER(shdZeppelinLight);

void ZeppelinScript::update(float delta)
{
	PhysicsComponent *phys = em_.getPhysicsComponent(id_);

	phys->torque = 0;
	phys->thrust = 0;
	// todo: left-right arbitration
	if (keyIsDown(KEY_LEFT)) {
		phys->torque = m_torque_;
	} else if (keyIsDown(KEY_RIGHT)) {
		phys->torque = -m_torque_;
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

	// todo: clipping
	for (int y = -charRadius; y <= charRadius; ++y) {
		for (int x = -charRadius; x <= charRadius; ++x) {
			vec2f c = vec2f((float)x * FONTH, (float)y * FONTH);
			float len = c.length() - FONTH / 2;
			if (len > radius) continue;
			float dp = dot(orient, c / len);
			if (dp < cosTheta) continue;

			// hilight
			Symbol sym;
			if (getSymbol(pos->x + x, pos->y + y, &sym)) {
				int fg = sym.fg + 16 < N_COLORS ? sym.fg + 16 : sym.fg;
				putAttrs((int)pos->x + x, (int)pos->y + y, fg, sym.bg);
			}
		}
	}
}
