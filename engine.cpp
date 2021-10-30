#include "engine.h"
#include <sstream>

float dot(vec2f lhs, vec2f rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

vec2f operator*(vec2f lhs, float rhs)
{
	return vec2f(lhs.x * rhs, lhs.y * rhs);
}

vec2f operator*(float lhs, vec2f rhs)
{
	return vec2f(lhs * rhs.x, lhs * rhs.y);
}

vec2f operator/(vec2f lhs, float rhs)
{
	return vec2f(lhs.x / rhs, lhs.y / rhs);
}

std::string getTextAssetFileName(const std::string& name)
{
	char buf[_MAX_PATH] = { 0 };
	snprintf(buf, _MAX_PATH, "assets\\%s.txt", name.c_str());
	return buf;
}

std::string getMapAssetFileName(const std::string& name)
{
	char buf[_MAX_PATH] = { 0 };
	snprintf(buf, _MAX_PATH, "assets\\%s.map.skii.txt", name.c_str());
	return buf;
}
