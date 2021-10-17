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
	SkiiImage tilemap;
	Registry<tid_t, ObjectDesc> objects;

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

LevelDesc loadLevel(const std::string& name);
