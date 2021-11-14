#include <cassert>
#include <cmath>
#include "ecs.h"
#include "level.h"

extern PhysicsConsts g_physConsts;
extern LevelDesc g_level;

Entity_t checkCollision(EntityManager& entityManager, Entity_t id, int x, int y)
{
	// todo: collision map

	// entities
	for (EntityManager::EntityData& d : entityManager.entities_) {
		if (d.id == id) continue;
		if ((int)d.pos.x == x && (int)d.pos.y == y) return d.id;
	}

	// static map
	Symbol tile = g_level.tilemap.at(x, y);
	for (char ch : g_level.colliderChars) {
		if (tile.ch == ch) return E_STATIC;
	}

	return E_UNDEF;
}

void handleCollision(EntityManager& entityManager, int x, int y, Entity_t source, Entity_t hit)
{
	ScriptComponent* sourceScript = entityManager.getScriptComponent(source);
	sourceScript->ps->onCollision(x, y, source, hit);
	if (hit > E_UNDEF) {
		ScriptComponent* hitScript = entityManager.getScriptComponent(hit);
		hitScript->ps->onCollision(x, y, source, hit);
	}
}

void updateEntityPhysics(EntityManager& entityManager, Entity_t id, PositionComponent& pos,
	PhysicsComponent& phys, float delta)
{
	if (phys.m == 0) return;

	float ldrag = phys.drag * phys.linearSpeed;
	float la = (phys.thrust - ldrag) / phys.m;
	phys.linearSpeed = std::min(phys.linearSpeed + la * delta, g_physConsts.maxLinearSpeed);
	float newX = pos.x + phys.linearSpeed * cosf(pos.theta) * delta;
	float newY = pos.y + phys.linearSpeed * sinf(pos.theta) * delta;
	Entity_t hit = checkCollision(entityManager, id, (int)newX, (int)newY);
	if (hit == E_UNDEF) {
		pos.x = newX;
		pos.y = newY;
	} else {
		handleCollision(entityManager, (int)newX, (int)newY, id, hit);
	}

	float adrag = phys.drag * phys.angularSpeed;
	float aa = (phys.torque - adrag) / phys.m;
	phys.angularSpeed = std::min(phys.angularSpeed + aa * delta, g_physConsts.maxAngularSpeed);
	pos.theta += phys.angularSpeed;
	if (pos.theta < 0.0f) {
		pos.theta = 2 * PI + pos.theta;
	} else if (pos.theta > 2 * PI) {
		pos.theta = pos.theta - 2 * PI;
	}

	phys.torque = 0;
	phys.thrust = 0;
}

void physicsSystem(EntityManager& entityManager, float delta)
{
	for (EntityManager::EntityData& d : entityManager.entities_) {
		updateEntityPhysics(entityManager, d.id, d.pos, d.phys, delta);
	}
}
