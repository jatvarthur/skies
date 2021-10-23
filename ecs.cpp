#include <cassert>
#include "ecs.h"

void EntityManager::addEntity(Entity_t id)
{
	assert(id > E_UNDEF);

	int index = id - 1;
	if (index < (int)entities_.size()) return;
	entities_.resize(id);
}

void EntityManager::clear()
{
	entities_.clear();
}

void renderSystem(const EntityManager& entityManager)
{
	for (const EntityManager::EntityData& d : entityManager.entities_) {
		drawChar(d.pos.x, d.pos.y, d.view.ch, d.view.fg, d.view.bg);
	}
}

void scriptSystem(const EntityManager& entityManager, int delta)
{

}

void physicsSystem(const EntityManager& entityManager, int delta)
{

}
