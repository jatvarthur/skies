#include <fstream>
#include "engine.h"
#include "render.h"
#include "keybrd.h"
#include "level.h"
#include "ecs.h"


int g_windowWidth = 0;
int g_windowHeight = 0;
LevelDesc g_level;
EntityManager g_entityManager;

const char* GAME_FILE_NAME = "game";

bool gameInit()
{
	std::ifstream gameFile(getTextAssetFileName(GAME_FILE_NAME));
	if (!gameFile) {
		return false;
	}

	gameFile >> g_windowWidth >> g_windowHeight;
	std::string levelName;
	gameFile >> levelName;
	loadLevel(levelName, g_level, g_entityManager);

	return true;
}

void gameStep(int delta)
{	
	if (keyIsPressed(KEY_F5)) {
		gameInit();
	}

	scriptSystem(g_entityManager, delta);
	physicsSystem(g_entityManager, delta);

	// "Logical" rendering
	// 1. tilemap, first layer
	drawImage(0, 0, 0, 0, 
		g_level.tilemap.width(), 
		g_level.tilemap.height(), 
		g_level.tilemap
	);

	// 2. game objects, second layer
	renderSystem(g_entityManager);

	// 3. UI, third layer
	// todo:

	int y = 62, x = 3;
	for (int i = 1; i < 256; ++i, ++x) {
		if (i % 16 == 0) {
			y += 1;
			x = 2;
		}
		drawChar(x, y, i, COLOR_BRIGHT_GREEN, COLOR_BLUE);
	}
}

void gameClean()
{

}
