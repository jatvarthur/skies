#include "level.h"
#include <fstream>
#include <cstdlib>
#include "ecs.h"


bool readPosition(std::istream& fin, PositionComponent* c)
{
	fin >> c->x >> c->y >> c->orientation;
	return fin.good();
}

bool readPhysics(std::istream& fin, PhysicsComponent* c)
{
	fin >> c->m;
	c->force = vec2f();
	c->velocity = vec2f();
	c->phi = 0.0f;
	return fin.good();
}

bool readRender(std::istream& fin, RenderComponent* c)
{
	int fg, bg;
	fin >> c->ch >> fg >> bg;
	c->fg = fg;
	c->bg = bg;
	return fin.good();
}

bool readScript(std::istream& fin, ScriptComponent* s)
{
	std::string script;
	int nParams;
	fin >> script >> nParams;
	// todo: 
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
			result = readPosition(fin, entityManager.getPositionComponent(id));
			break;
		case 'Y':
			result = readPhysics(fin, entityManager.getPhysicsComponent(id));
			break;
		case 'R':
			result = readRender(fin, entityManager.getRenderComponent(id));
			break;
		case 'S':
			result = readScript(fin, entityManager.getScriptComponent(id));
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
