#include <cassert>
#include <cmath>
#include "ecs.h"


extern PhysicsConsts g_physConsts;

void updateEntityPhysics(PositionComponent& pos, PhysicsComponent& phys, float delta)
{
	if (phys.m == 0) return;

	float ldrag = g_physConsts.B * phys.linearSpeed;
	float la = (phys.thrust - ldrag) / phys.m;
	phys.linearSpeed = std::min(phys.linearSpeed + la * delta, g_physConsts.maxLinearSpeed);
	pos.x += phys.linearSpeed * cosf(pos.theta) * delta;
	pos.y += phys.linearSpeed * sinf(pos.theta) * delta;

	float adrag = g_physConsts.B * phys.angularSpeed;
	float aa = (phys.torque - adrag) / phys.m;
	phys.angularSpeed = std::min(phys.angularSpeed + aa * delta,g_physConsts.maxAngularSpeed);
	pos.theta += phys.angularSpeed;
	if (pos.theta < 0.0f) {
		pos.theta = 2 * PI + pos.theta;
	} else if (pos.theta > 2 * PI) {
		pos.theta = pos.theta - 2 * PI;
	}
}

ScriptRegistry& ScriptRegistry::get()
{
	static ScriptRegistry instance;
	return instance;
}

void ScriptRegistry::registerFactory(const std::string& name, ScriptFactoryProc_t factoryProc)
{
	registry_[name] = factoryProc;
}

Script* ScriptRegistry::create(const std::string& name, EntityManager& em, Entity_t id) const
{
	auto it = registry_.find(name);
	assert(it != registry_.end());

	return it->second(em, id);
}


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

Shader* EntityManager::addShader(Entity_t id)
{
	Shader shader = { 0 };
	shader.entityId = id;
	shaders_.push_back(shader);
	return &shaders_[shaders_.size() - 1];
}


void renderSystem(EntityManager& entityManager)
{
	// todo: clipping
	for (const EntityManager::EntityData& d : entityManager.entities_) {
		drawChar((int)d.pos.x, (int)d.pos.y, d.view.ch, d.view.fg, d.view.bg);
	}

	for (Shader& s : entityManager.shaders_) {
		s.shaderProc(&s, entityManager);
	}
}

void physicsSystem(EntityManager& entityManager, float delta)
{
	for (EntityManager::EntityData& d : entityManager.entities_) {
		updateEntityPhysics(d.pos, d.phys, delta);
	}
}

void scriptSystem(EntityManager& entityManager, float delta)
{
	for (EntityManager::EntityData& d : entityManager.entities_) {
		d.script.ps->update(delta);
	}
}

void scriptSystemAwake(EntityManager& entityManager)
{
	for (EntityManager::EntityData& d : entityManager.entities_) {
		d.script.ps->awake();
	}
}

void scriptSystemShutdown(EntityManager& entityManager)
{
	for (EntityManager::EntityData& d : entityManager.entities_) {
		d.script.ps->shutdown();
	}
}

void scriptSystemCleanup(EntityManager& entityManager)
{
	for (EntityManager::EntityData& d : entityManager.entities_) {
		delete d.script.ps;
		d.script.ps = nullptr;
	}
}
