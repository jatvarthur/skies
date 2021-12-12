#include "..\Zeppelin.h"
#include <cmath>
#include "..\..\engine\keybrd.h"


REGISTER_SCRIPT(ZeppelinScript);
REGISTER_SHADER(shdZeppelinLight);

static const std::string FILEDS_CARGO[] = {
	"n_alloy", "n_aether", "n_food", "n_luxury", "n_weapon"
};

void ZeppelinScript::awake()
{
	hudWindow_ = g_uiManager.showWindow("zeppelin_hud", nullptr);
	fieldMoney_ = (NumberField*)hudWindow_->findControl("n_money");
	for (int i = 0; i < N_RESOURCES; ++i) {
		fieldsCargo_[i] = (NumberField*)hudWindow_->findControl(FILEDS_CARGO[i]);
	}
}

void ZeppelinScript::update(float delta)
{
	PhysicsComponent *phys = em_.getPhysicsComponent(id_);

	bool isLeft = keyIsDown(KEY_LEFT);
	bool isRight = keyIsDown(KEY_RIGHT);

	if (isLeft && isRight) {
		// do nothing, as steering is cancelled out
		phys->torque = 0;
	} else if (isLeft) {
		phys->torque = -torque_;
	} else if (isRight) {
		phys->torque = torque_;
	}

	if (keyIsDown(KEY_DOWN)) {
		phys->thrust = -thrust_ / 3;
	} else if (keyIsDown(KEY_UP)) {
		phys->thrust = thrust_;
	}

	fieldMoney_->setValue(money());
	for (int i = 0; i < N_RESOURCES; ++i) {
		fieldsCargo_[i]->setValue(cargo(i));
	}
}

void ZeppelinScript::load(std::istream& is)
{
	is >> thrust_ >> torque_ >> maxCargoWeight_ >> money_;

	cargoWeight_ = 0;
	for (int i = 0; i < N_RESOURCES; ++i) {
		is >> cargo_[i];
		cargoWeight_ += cargo_[i];
	}

	assert(cargoWeight_ <= maxCargoWeight_);
}

void ZeppelinScript::buy(int name, int price)
{
	assert(name >= 0 && name < N_RESOURCES);
	assert(cargoWeight_ < maxCargoWeight_);
	
	pay(price);
	cargo_[name] += 1;
	cargoWeight_ += 1;
}

void ZeppelinScript::sell(int name, int price)
{
	assert(name >= 0 && name < N_RESOURCES);
	assert(cargo_[name] > 0);

	earn(price);
	cargo_[name] -= 1;
	cargoWeight_ -= 1;
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
