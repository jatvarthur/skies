#pragma once
#include <vector>
#include <unordered_map>
#include "render.h"
#include "engine.h"


typedef int Entity_t;
const Entity_t E_UNDEF = 0;
class EntityManager;

// script subsystem
class Script {
protected:
	Script(const Script& rhs) = delete;
	Script& operator=(const Script& rhs) = delete;

public:
	Script(EntityManager& em, Entity_t id)
		: em_(em), id_(id)
	{}
	virtual ~Script() {}

	virtual void awake() {}
	virtual void shutdown() {}
	virtual void update(float delta) {}

	virtual void load(std::istream& is) {}

protected:
	EntityManager& em_;
	Entity_t id_;
};

using ScriptFactoryProc_t = Script* (*)(EntityManager& em, Entity_t id);

class ScriptRegistry final {
protected:
	ScriptRegistry() {}

public:
	void registerFactory(const std::string& name, ScriptFactoryProc_t factoryProc);
	Script* create(const std::string& name, EntityManager& em, Entity_t id) const;

public:
	static ScriptRegistry& get();

private:
	std::unordered_map<std::string, ScriptFactoryProc_t> registry_;
};

#define REGISTER_SCRIPT(scriptClass) \
	static class scriptClass ## Factory {	\
	public:							\
		scriptClass ## Factory() { ScriptRegistry::get().registerFactory(#scriptClass, createScript); } \
		static Script* createScript(EntityManager& em, Entity_t id) { return new scriptClass(em, id); }	\
	} s_ ## scriptClass ## Factory


// shader subsystem
struct Shader;

using ShaderProc_t = void(*)(const Shader* shader, EntityManager& em);

struct Shader {
	ShaderProc_t shaderProc;
	Entity_t entityId;
	union {
		struct { float p0, p1, p2, p3, p4; };
		float p[5];
	};
};

class ShaderRegistry final {
protected:
	ShaderRegistry() {}

public:
	void registerShader(const std::string& name, ShaderProc_t shaderProc);
	ShaderProc_t getShader(const std::string& name) const;

public:
	static ShaderRegistry& get();

private:
	std::unordered_map<std::string, ShaderProc_t> registry_;
};

#define REGISTER_SHADER(shaderProc) \
	static class shaderProc ## Registrar {	\
	public:							\
		shaderProc ## Registrar() { ShaderRegistry::get().registerShader(#shaderProc, shaderProc); } \
	} s_ ## shaderProc ## Registrar


// physics subsystem
struct PhysicsConsts {
	float B;				// based on Stokes coeff 6*PI*ETA*r
	float maxLinearSpeed;	// in tiles
	float maxAngularSpeed;	// in radians
};


// Game Object + Components: is a restricted ECS approach
struct PositionComponent {
	float x, y;
	float theta;	// orientation
};

struct PhysicsComponent {
	int m;			// mass, 0 for static
	float thrust;   // provided by user
	float torque;
	float linearSpeed;
	float angularSpeed;
};

struct RenderComponent {
	schar_t ch;
	color_t fg;
	color_t bg;
	// todo: shader?
};

struct ScriptComponent {
	Script* ps;
};


class EntityManager final
{
private:
	friend void renderSystem(EntityManager& entityManager);
	friend void physicsSystem(EntityManager& entityManager, float delta);
	friend void scriptSystem(EntityManager& entityManager, float delta);
	friend void scriptSystemAwake(EntityManager& entityManager);
	friend void scriptSystemShutdown(EntityManager& entityManager);
	friend void scriptSystemCleanup(EntityManager& entityManager);

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

	Shader* addShader(Entity_t id);

private:
	// indexed by entityId
	std::vector<EntityData> entities_;
	// not by entityId
	std::vector<Shader> shaders_;
};

void renderSystem(EntityManager& entityManager);
void physicsSystem(EntityManager& entityManager, float delta);
void scriptSystem(EntityManager& entityManager, float delta);
void scriptSystemAwake(EntityManager& entityManager);
void scriptSystemShutdown(EntityManager& entityManager);
void scriptSystemCleanup(EntityManager& entityManager);
