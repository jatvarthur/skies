#pragma once
#include <map>
#include <cassert>
#include <string>
#include <cmath>

#define NELEMS(a) (sizeof(a) / sizeof(a[0]))

/*
template<typename ID, typename T>
class Registry {
private:
	typedef std::map<ID, T> ItemsMap;
public:
	const T& get(ID id) const;
	void put(ID id, const T& item);

private:
	ItemsMap items_;
};

template<typename ID, typename T>
const T& Registry<ID, T>::get(ID id) const
{
	typename ItemsMap::const_iterator it = items_.find(id);
	assert(it != items_.end());
	return it->second;
}

template<typename ID, typename T>
void Registry<ID, T>::put(ID id, const T& item)
{
	items_[id] = item;
}

*/

const float PI = 3.1415926f;

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

float dot(vec2f lhs, vec2f rhs);
vec2f operator*(vec2f lhs, float rhs);
vec2f operator*(float lhs, vec2f rhs);
vec2f operator/(vec2f lhs, float rhs);




std::string getTextAssetFileName(const std::string& name);
std::string getMapAssetFileName(const std::string& name);
