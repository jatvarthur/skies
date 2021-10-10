#include "level.h"
#include <fstream>
#include <cstdlib>

std::shared_ptr<LevelDesc> loadLevel(const std::string& fileName)
{
	std::ifstream levelFile(fileName);
	if (!levelFile) {
		return nullptr;
	}

	std::shared_ptr<LevelDesc> desc = std::make_shared<LevelDesc>();
	levelFile >> desc->width >> desc->height;
	int fg, bg;
	levelFile >> fg >> bg;
	desc->defaultFg = fg;
	desc->defaultBg = bg;

	int nTiles = desc->width * desc->height;
	desc->map.reserve(nTiles);
	std::vector<char> buffer(desc->width + 1, '\8');
	for (int j = 0; j < desc->height; ++j) {
		levelFile.ignore(INT_MAX, '\n');
		levelFile.getline(&buffer[0], desc->width + 1, '\n');
		std::copy(buffer.begin(), buffer.begin() + desc->width, std::back_inserter(desc->map));
	}

	int nObjects;
	levelFile >> nObjects;
}
