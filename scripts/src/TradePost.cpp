#include "..\TradePost.h"


REGISTER_SCRIPT(TradePostScript);

const float UPDATE_STEP = 60.0;
const Entity_t E_PLAYER = 1;


void TradePostScript::awake()
{
	timeToUpdate_ = 0.0f;
}

void TradePostScript::update(float delta)
{
	timeToUpdate_ += delta;
	if (timeToUpdate_ >= UPDATE_STEP) {
		_debug("Trade post #%d:\n", id_);
		for (int i = 0; i < N_RESOURCES; ++i) {
			ResourceData& r = resources_[i];
			r.stock = std::min(r.maxStock, std::max(0, r.stock + r.rate));
			_debug("	[%d]: stock=%d\n", i, resources_[i].stock);
		}
		timeToUpdate_ = 0.0f;
	}
}

void TradePostScript::load(std::istream& is)
{
	for (int i = 0; i < N_RESOURCES; ++i) {
		is >> resources_[i].stock 
			>> resources_[i].rate
			>> resources_[i].maxStock
			>> resources_[i].minPrice 
			>> resources_[i].maxPrice;
	}
}

void TradePostScript::onCollision(int x, int y, Entity_t source, Entity_t hit)
{
	if (source == E_PLAYER) {
		PhysicsComponent* phys = em_.getPhysicsComponent(source);
		phys->linearSpeed = 0;
		phys->angularSpeed = 0;

		g_uiManager.showModal("trade_post", (UiEventListener*)this);
	}
}

// ! order of items should conform to enum Resource
static const std::string FILEDS_STOCK[] = {
	"n_alloy_st", "n_aether_st", "n_food_st", "n_luxury_st", "n_weapon_st"
};
static const std::string FILEDS_CARGO[] = {
	"n_alloy_cr", "n_aether_cr", "n_food_cr", "n_luxury_cr", "n_weapon_cr"
};
static const std::string FILEDS_BUY[] = {
	"n_alloy_b", "n_aether_b", "n_food_b", "n_luxury_b", "n_weapon_b"
};
static const std::string FILEDS_SELL[] = {
	"n_alloy_s", "n_aether_s", "n_food_s", "n_luxury_s", "n_weapon_s"
};

void TradePostScript::onPrepare(Window* window)
{
	ZeppelinScript* player = getPlayer();
	NumberField* nf;
	for (int i = 0; i < N_RESOURCES; ++i) {
		nf = (NumberField*)window->findControl(FILEDS_STOCK[i]);
		nf->setValue(resources_[i].stock);

		nf = (NumberField*)window->findControl(FILEDS_CARGO[i]);
		nf->setValue(player->getCargoResource(i));

		// buy for player, sell for trade post 
		nf = (NumberField*)window->findControl(FILEDS_BUY[i]);
		nf->setValue(getSellPrice(i, resources_[i].stock));

		// sell for player, buy for thrade post
		nf = (NumberField*)window->findControl(FILEDS_SELL[i]);
		nf->setValue(getSellPrice(i, resources_[i].stock + 1));
	}
}

void TradePostScript::onClick(Window* window, Control* sender)
{
	_debug("Clicked: %s\n", sender->id().c_str());
}

ZeppelinScript* TradePostScript::getPlayer()
{
	ScriptComponent* sc = em_.getScriptComponent(E_PLAYER);
	assert(sc->ps && sc->ps->getScriptType() == "ZeppelinScript");

	return (ZeppelinScript*)sc->ps;
}

int TradePostScript::getSellPrice(int name, int stock)
{
	assert(name >= 0 && name < N_RESOURCES);
	ResourceData& r = resources_[name];
	if (r.maxStock == 0 || stock == 0 || stock > r.maxStock) {
		return 0;
	}

	return r.maxPrice - (r.maxPrice - r.minPrice) * stock / r.maxStock;
}
