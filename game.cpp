#include <fstream>
#include "engine.h"
#include "render.h"
#include "level.h"


int g_windowWidth = 0;
int g_windowHeight = 0;
std::shared_ptr<LevelDesc> g_level;

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
	g_level = loadLevel(getTextAssetFileName(levelName));

	return true;
}

void gameStep()
{
	drawString(96, 10, "Skies .1", COLOR_BRIGHT_YELLOW, COLOR_MAGENTA);
	
	int y = 12, x = 11;
	for (int i = 1; i < 256; ++i, ++x) {
		if (i % 16 == 0) {
			y += 1;
			x = 10;
		}
		drawChar(x, y, i, COLOR_BRIGHT_GREEN, COLOR_BLUE);

	}
}

void gameClean()
{

}
