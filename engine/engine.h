#pragma once
#include <map>
#include <cassert>
#include <string>
#include <vector>
#include <cmath>

#define NELEMS(a) (sizeof(a) / sizeof(a[0]))

const float PI = 3.1415926f;
const float EPS = 0.0001f;

struct vec2i {
	int x, y;

	vec2i() : x(0), y(0) {}
	vec2i(int ax, int ay) : x(ax), y(ay) {}

};

struct vec2f {
	float x, y;

	vec2f() : x(0.0f), y(0.0f) {}
	vec2f(float ax, float ay) : x(ax), y(ay) {}

	float length() const 
	{ 
		return sqrt(x * x + y * y); 
	}

	vec2f norm() const
	{
		float len = length();
		return vec2f(x / len, y / len);
	}
};

inline float dot(vec2f lhs, vec2f rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

inline vec2f operator*(vec2f lhs, float rhs)
{
	return vec2f(lhs.x * rhs, lhs.y * rhs);
}

inline vec2f operator*(float lhs, vec2f rhs)
{
	return vec2f(lhs * rhs.x, lhs * rhs.y);
}

inline vec2f operator/(vec2f lhs, float rhs)
{
	return vec2f(lhs.x / rhs, lhs.y / rhs);
}

void _debug(const char* fmt, ...);

static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
}

std::string getTextAssetFileName(const std::string& name);
std::string getMapAssetFileName(const std::string& name);

void loadString(std::istream& is, std::string& str, bool fromNewLine);
void loadText(std::istream& is, std::vector<std::string>& text);
