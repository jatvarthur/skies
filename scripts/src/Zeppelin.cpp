#include "..\Zeppelin.h"
#include <cmath>
#include "..\..\keybrd.h"

REGISTER_SCRIPT(zeppelin, ZeppelinScript);
REGISTER_SHADER(zeppelinLight, zeppelinLightShader);


void ZeppelinScript::update(float delta)
{
	PositionComponent *pos = em_.getPositionComponent(id_);
	if (keyIsDown(KEY_LEFT)) {
		pos->orientation += 0.05f;
	} else if (keyIsDown(KEY_RIGHT)) {
		pos->orientation -= 0.05f;
	}
}



const int FONTH = 8;

void zeppelinLightShader(const Shader* shader, EntityManager& em)
{
	PositionComponent* pos = em.getPositionComponent(shader->entityId);
	float cosTheta = cos(shader->p0 * PI / 180.0f);
	int charRadius = (int)shader->p1;
	float radius = (float)charRadius * FONTH;
	vec2f orient = vec2f(cosf(pos->orientation), sinf(pos->orientation));

	// todo: clipping
	for (int y = -charRadius; y <= charRadius; ++y) {
		for (int x = -charRadius; x <= charRadius; ++x) {
			vec2f c = vec2f((float)x * FONTH, (float)y * FONTH);
			float len = c.length();
			if (len > radius) continue;
			float dp = dot(orient, c / len);
			if (dp < cosTheta) continue;

			// hilight
			Symbol sym = getSymbol(pos->x + x, pos->y + y);
			int fg = sym.fg + 16 < N_COLORS ? sym.fg + 16 : sym.fg;
			putAttrs(pos->x + x, pos->y + y, fg, sym.bg);
		}
	}
}
