#include "level.h"
#include <fstream>
#include <cassert>
#include <cstdlib>
#include "ecs.h"


bool readPosition(std::istream& fin, Entity_t id, EntityManager& entityManager)
{
	PositionComponent* c = entityManager.getPositionComponent(id);

	float o;
	fin >> c->x >> c->y >> o;
	c->orientation = o * PI / 180.0f;
	return fin.good();
}

bool readPhysics(std::istream& fin, Entity_t id, EntityManager& entityManager)
{
	PhysicsComponent* c = entityManager.getPhysicsComponent(id);

	fin >> c->m;
	c->force = vec2f();
	c->velocity = vec2f();
	c->phi = 0.0f;
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
		int paramsNo;
		fin >> shaderName >> paramsNo;
		Shader* shader = entityManager.addShader(id);
		assert(paramsNo <= NELEMS(shader->p));

		shader->shaderProc = ShaderRegistry::get().getShader(shaderName);
		for (int i = 0; i < paramsNo; ++i) {
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
