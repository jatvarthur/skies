#pragma once
#include <vector>
#include "render.h"
#include "engine.h"

// Game Object + Components: is a restricted ECS approach
struct PositionComponent {
	int x, y;
	int orientation;
};

struct PhysicsComponent {
	float m;		// mass
	vec2f force;
	vec2f velocity;
	float phi;		// angular speed
};

struct RenderComponent {
	schar_t ch;
	color_t fg;
	color_t bg;
	// todo: shader?
};

struct ScriptComponent {
	// pointer to script
};

typedef int Entity_t;
const Entity_t E_UNDEF = 0;

class EntityManager final
{
private:
	friend void renderSystem(const EntityManager& entityManager);
	friend void scriptSystem(const EntityManager& entityManager, int delta);
	friend void physicsSystem(const EntityManager& entityManager, int delta);

	struct EntityData {
		PositionComponent pos;
		PhysicsComponent phys;
		RenderComponent view;
		ScriptComponent script;
	};

public:
	EntityManager() {}
	~EntityManager() {}

	void addEntity(Entity_t id);
	void clear();

	PositionComponent* getPositionComponent(Entity_t id)
	{
		assert(id > E_UNDEF && id <= (int)entities_.size());
		return &entities_[id - 1].pos;
	}

	PhysicsComponent* getPhysicsComponent(Entity_t id)
	{
		assert(id > E_UNDEF && id <= (int)entities_.size());
		return &entities_[id - 1].phys;
	}

	RenderComponent* getRenderComponent(Entity_t id)
	{
		assert(id > E_UNDEF && id <= (int)entities_.size());
		return &entities_[id - 1].view;
	}

	ScriptComponent* getScriptComponent(Entity_t id)
	{
		assert(id > E_UNDEF && id <= (int)entities_.size());
		return &entities_[id - 1].script;
	}

private:
	std::vector<EntityData> entities_;
};

void renderSystem(const EntityManager& entityManager);
void scriptSystem(const EntityManager& entityManager, int delta);
void physicsSystem(const EntityManager& entityManager, int delta);