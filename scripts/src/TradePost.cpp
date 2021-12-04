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

static const std::string FIELD_PLAYER_WEIGHT = "n_player_weight";
static const std::string FIELD_PLAYER_M_WEIGHT = "n_player_m_weight";
static const std::string FIELD_PLAYER_MONEY = "n_player_money";
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
static const std::string BUTTONS_BUY[] = {
	"b_alloy_b", "b_aether_b", "b_food_b", "b_luxury_b", "b_weapon_b"
};
static const std::string BUTTONS_SELL[] = {
	"b_alloy_s", "b_aether_s", "b_food_s", "b_luxury_s", "b_weapon_s"
};

int findResourceByButton(const std::string& id, const std::string buttons[], int nButtons)
{
	for (int i = 0; i < nButtons; ++i) {
		if (id == buttons[i]) {
			return i;
		}
	}
	return -1;
}


void TradePostScript::updateControls(Window* window)
{
	ZeppelinScript* player = getPlayer();
	NumberField* nf;
	Button* b;
	for (int i = 0; i < N_RESOURCES; ++i) {
		nf = (NumberField*)window->findControl(FILEDS_STOCK[i]);
		nf->setValue(resources_[i].stock);

		nf = (NumberField*)window->findControl(FILEDS_CARGO[i]);
		nf->setValue(player->cargo(i));

		int stock = resources_[i].stock;
		// buy for player, sell for trade post
		int sellPrice = getSellPrice(i, stock);
		bool canBuy = stock > 0 && player->money() >= sellPrice 
			&& player->cargoWeight() < player->maxCargoWeight();	
		nf = (NumberField*)window->findControl(FILEDS_BUY[i]);
		nf->setValue(sellPrice);
		b = (Button*)window->findControl(BUTTONS_BUY[i]);
		window->enableControl(b, canBuy);

		// sell for player, buy for trade post
		int buyPrice = getSellPrice(i, stock + 1);
		bool canSell = player->cargo(i) > 0 && stock < resources_[i].maxStock;
		nf = (NumberField*)window->findControl(FILEDS_SELL[i]);
		nf->setValue(buyPrice);
		b = (Button*)window->findControl(BUTTONS_SELL[i]);
		window->enableControl(b, canSell);
	}

	nf = (NumberField*)window->findControl(FIELD_PLAYER_WEIGHT);
	nf->setValue(player->cargoWeight());
	nf = (NumberField*)window->findControl(FIELD_PLAYER_M_WEIGHT);
	nf->setValue(player->maxCargoWeight());
	nf = (NumberField*)window->findControl(FIELD_PLAYER_MONEY);
	nf->setValue(player->money());
}

void TradePostScript::onPrepare(Window* window)
{
	updateControls(window);
}

void TradePostScript::onClick(Window* window, Control* sender)
{
	_debug("Clicked: %s\n", sender->id().c_str());

	int res = findResourceByButton(sender->id(), BUTTONS_BUY, NELEMS(BUTTONS_BUY));
	if (res != -1) {
		sellToPlayer(res);
		updateControls(window);
		return;
	}

	res = findResourceByButton(sender->id(), BUTTONS_SELL, NELEMS(BUTTONS_SELL));
	if (res != -1) {
		buyFromPlayer(res);
		updateControls(window);
		return;
	}
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

void TradePostScript::sellToPlayer(int name)
{
	assert(resources_[name].stock > 0);

	ZeppelinScript* player = getPlayer();
	int price = getSellPrice(name, resources_[name].stock);

	player->buy(name, price);
	resources_[name].stock -= 1;
}

void TradePostScript::buyFromPlayer(int name)
{
	assert(resources_[name].stock < resources_[name].maxStock);

	ZeppelinScript* player = getPlayer();
	int price = getSellPrice(name, resources_[name].stock + 1);

	player->sell(name, price);
	resources_[name].stock += 1;
}
