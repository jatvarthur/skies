#include <fstream>
#include <cmath>
#include "game.h"
#include "engine.h"
#include "render.h"
#include "keybrd.h"
#include "level.h"
#include "ecs.h"


int g_windowWidth = 0;
int g_windowHeight = 0;
LevelDesc g_level;
EntityManager g_entityManager;
PhysicsConsts g_physConsts;
FixedCamera g_camera(g_entityManager);

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

	int x, y;
	gameFile >> x >> y;
	g_camera.setCenter(vec2i(x, y));

	return true;
}

bool gameAwake()
{
	scriptSystemAwake(g_entityManager);
	return true;
}

void gameStep(float delta)
{	
	if (keyIsPressed(KEY_F5)) {
		gameShutdown();
		gameClean();
		gameInit();
		gameAwake();
	}

	scriptSystem(g_entityManager, delta);
	physicsSystem(g_entityManager, delta);

	// "Logical" rendering
	// 1. tilemap, first layer

	vec2i t = g_camera.translation();
	int xDst = 0, yDst = 0;
	int xSrc = 0, ySrc = 0;
	int wSrc = 0, hSrc = 0;
	if (t.x < 0) {
		xSrc = -t.x;
		wSrc = std::min(g_level.tilemap.width() + xSrc, g_windowWidth);
	} else {
		xDst = t.x;
		wSrc = std::min(g_level.tilemap.width(), g_windowWidth - xDst);
	}
	if (t.y < 0) {
		ySrc = -t.y;
		hSrc = std::min(g_level.tilemap.height() + ySrc, g_windowHeight);
	} else {
		yDst = t.y;
		hSrc = std::min(g_level.tilemap.height(), g_windowHeight - yDst);
	}

	drawImage(xDst, yDst, xSrc, ySrc, wSrc, hSrc, g_level.tilemap);

	// 2. game objects, second layer
	drawTranslate(t.x, t.y);
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

void gameShutdown()
{
	scriptSystemShutdown(g_entityManager);
}

void gameClean()
{
	scriptSystemCleanup(g_entityManager);
}
