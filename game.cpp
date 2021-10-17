#include <fstream>
#include "engine.h"
#include "render.h"
#include "level.h"


int g_windowWidth = 0;
int g_windowHeight = 0;
LevelDesc g_level;

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
	g_level = loadLevel(levelName);

	return true;
}

void gameStep()
{	
	drawImage(0, 0, 0, 0, 
		g_level.tilemap.width(), 
		g_level.tilemap.height(), 
		g_level.tilemap
	);

	
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
