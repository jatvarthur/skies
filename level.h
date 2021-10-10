#pragma once
#include <vector>
#include <memory>
#include "engine.h"
#include "render.h"

// Object Type Id
typedef char tid_t;

struct ObjectDesc {
	tid_t id;
};

struct LevelDesc {
	int width;
	int height;
	color_t defaultBg;
	color_t defaultFg;
	std::vector<tid_t> map;
	Registry<tid_t, ObjectDesc> objects;
};

std::shared_ptr<LevelDesc> loadLevel(const std::string& fileName);
