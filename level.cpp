#include "level.h"
#include <fstream>
#include <cstdlib>

LevelDesc loadLevel(const std::string& name)
{
	std::ifstream levelFile(getTextAssetFileName(name));
	if (!levelFile) {
		return LevelDesc(0, 0, SkiiImage::invalid());
	}

	int w, h;
	levelFile >> w >> h;
	std::string tmName;
	levelFile >> tmName;
	std::ifstream tilemapFile(getMapAssetFileName(tmName));
	if (!levelFile) {
		return LevelDesc(0, 0, SkiiImage::invalid());
	}

	SkiiImage tilemap = SkiiImage::loadFromStream(tilemapFile);
	if (!tilemap) {
		return LevelDesc(0, 0, SkiiImage::invalid());
	}

	LevelDesc result(w, h, tilemap);

	int nObjects;
	levelFile >> nObjects;
	// todo:

	return result;
}
