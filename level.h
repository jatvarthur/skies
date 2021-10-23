#pragma once
#include <vector>
#include <memory>
#include "engine.h"
#include "render.h"
#include "ecs.h"

struct LevelDesc {
	int width;
	int height;
	SkiiImage tilemap;


	// todo: ?
	LevelDesc()
		: width(0)
		, height(0)
		, tilemap(SkiiImage::invalid())
	{}

	LevelDesc(int w, int h, const SkiiImage& tm)
		: width(w)
		, height(h)
		, tilemap(tm)
	{}
};

bool loadLevel(const std::string& name, LevelDesc& levelDesc, EntityManager& entityManager);
