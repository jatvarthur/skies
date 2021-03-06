#include <fstream>
#include <cmath>
#include "..\game.h"
#include "..\engine.h"
#include "..\render.h"
#include "..\keybrd.h"
#include "..\level.h"
#include "..\ecs.h"
#include "..\ui.h"


int g_windowWidth = 0;
int g_windowHeight = 0;
LevelDesc g_level;
EntityManager g_entityManager;
UiManager g_uiManager;
PhysicsConsts g_physConsts;
FixedCamera g_camera(g_entityManager);

extern int g_fps;
bool g_showStats = false;


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

	std::ifstream uiFile(getTextAssetFileName("level1.ui"));
	g_uiManager.load(uiFile);

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

	if (keyIsPressed(KEY_F1)) {
		g_showStats = !g_showStats;
	}

	if (g_uiManager.preUpdate(delta)) {
		scriptSystem(g_entityManager, delta);
		physicsSystem(g_entityManager, delta);
		g_uiManager.update(delta);
	}

	// "Logical" rendering
	drawIdentity();
	g_camera.apply();

	// 1. tilemap, first layer
	drawImage(0, 0, 0, 0, g_level.tilemap.width(), g_level.tilemap.height(), g_level.tilemap);
	// 2. game objects, second layer
	renderSystem(g_entityManager);
	// 3. UI, third layer
	g_uiManager.render();

	if (g_showStats) {
		drawIdentity();
		int y = 62, x = 3;
		for (int i = 1; i < 256; ++i, ++x) {
			if (i % 16 == 0) {
				y += 1;
				x = 2;
			}
			drawChar(x, y, i, COLOR_BRIGHT_GREEN, COLOR_BLUE);
		}

		char buf[40] = { 0 };
		snprintf(buf, 40, "%dfps", g_fps);
		drawString(2, 2, buf, COLOR_BRIGHT_GREEN, COLOR_BLUE);
	}
}

void gameShutdown()
{
	scriptSystemShutdown(g_entityManager);
	g_uiManager.shutdown();
}

void gameClean()
{
	scriptSystemCleanup(g_entityManager);
}
