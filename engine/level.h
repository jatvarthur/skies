#pragma once
#include <vector>
#include <memory>
#include "engine.h"
#include "render.h"
#include "ecs.h"


class FixedCamera {
public:
	FixedCamera(EntityManager& em)
		: em_(em) 
	{}

	void setCenter(vec2i value)
	{
		wCenter_ = value;
	}

	void setEntity(Entity_t value)
	{
		id_ = value;
	}

	vec2i translation() const
	{
		assert(id_ != E_UNDEF);
		
		PositionComponent *pos = em_.getPositionComponent(id_);
		return vec2i(wCenter_.x - (int)pos->x, wCenter_.y - (int)pos->y);
	}

	void apply()
	{
		vec2i t = translation();
		drawTranslate(t.x, t.y);
	}

private:
	vec2i wCenter_;
	Entity_t id_;
	EntityManager& em_;
};

struct LevelDesc {
	int width;
	int height;
	SkiiImage tilemap;
	std::vector<char> colliderChars;

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
