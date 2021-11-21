#include "level.h"
#include <fstream>
#include <cassert>
#include <cstdlib>
#include "ecs.h"


extern PhysicsConsts g_physConsts;
extern FixedCamera g_camera;

bool readPosition(std::istream& fin, Entity_t id, EntityManager& entityManager)
{
	PositionComponent* c = entityManager.getPositionComponent(id);

	int x, y;
	float o;
	fin >> x >> y >> o;
	c->x = (float)x + 0.5f;
	c->y = (float)y + 0.5f;
	c->theta = o * PI / 180.0f;
	return fin.good();
}

bool readPhysics(std::istream& fin, Entity_t id, EntityManager& entityManager)
{
	PhysicsComponent* c = entityManager.getPhysicsComponent(id);
	memset(c, 0, sizeof(*c));

	fin >> c->m >> c->drag;
	return fin.good();
}

bool readRender(std::istream& fin, Entity_t id, EntityManager& entityManager)
{
	RenderComponent* c = entityManager.getRenderComponent(id);

	int fg, bg, hasShader;
	fin >> c->ch >> fg >> bg >> hasShader;
	c->fg = fg;
	c->bg = bg;

	if (hasShader) {
		std::string shaderName;
		int nParams;
		fin >> shaderName >> nParams;
		Shader* shader = entityManager.addShader(id);
		assert(nParams <= NELEMS(shader->p));

		shader->shaderProc = ShaderRegistry::get().getShader(shaderName);
		for (int i = 0; i < nParams; ++i) {
			fin >> shader->p[i];
		}
	}
	return fin.good();
}

bool readScript(std::istream& fin, Entity_t id, EntityManager& entityManager)
{
	ScriptComponent* s = entityManager.getScriptComponent(id);

	std::string scriptName;
	fin >> scriptName;
	s->ps = ScriptRegistry::get().create(scriptName, entityManager, id);
	s->ps->load(fin);
	return fin.good();
}


bool readObject(std::istream& fin, EntityManager& entityManager)
{
	int id;
	fin >> id;
	entityManager.addEntity(id);

	bool result = false;
	for (int i = 0; i < 4; ++i) { // Number_of_compoments
		char ch;
		fin >> ch;
		switch (ch)
		{
		case 'P':
			result = readPosition(fin, id, entityManager);
			break;
		case 'Y':
			result = readPhysics(fin, id, entityManager);
			break;
		case 'R':
			result = readRender(fin, id, entityManager);
			break;
		case 'S':
			result = readScript(fin, id, entityManager);
			break;
		default:
			break;
		}
		if (!result) return false;
	}
	return true;
}

bool loadLevel(const std::string& name, LevelDesc& levelDesc, EntityManager& entityManager)
{
	std::ifstream levelFile(getTextAssetFileName(name));
	if (!levelFile) {
		return false;
	}

	int w, h;
	levelFile >> w >> h;
	std::string tmName;
	levelFile >> tmName;
	std::ifstream tilemapFile(getMapAssetFileName(tmName));
	if (!levelFile) {
		return false;
	}

	SkiiImage tilemap = SkiiImage::loadFromStream(tilemapFile);
	if (!tilemap) {
		return false;
	}

	levelDesc.width = w;
	levelDesc.height = h;
	levelDesc.tilemap = tilemap;

	levelFile >> g_physConsts.maxLinearSpeed >> g_physConsts.maxAngularSpeed;
	
	int nColliderChars;
	levelFile >> nColliderChars;
	levelDesc.colliderChars.resize(nColliderChars);
	for (int i = 0; i < nColliderChars; ++i) {
		levelFile >> levelDesc.colliderChars[i];
	}

	Entity_t playerId;
	levelFile >> playerId;
	g_camera.setEntity(playerId);

	int nObjects;
	levelFile >> nObjects;

	for (int i = 0; i < nObjects; ++i) {
		if (!readObject(levelFile, entityManager)) {
			assert(false && "Unable to read objects");
			entityManager.clear();
			return false;
		}
	}

	return true;
}
