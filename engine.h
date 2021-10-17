#pragma once
#include <map>
#include <cassert>
#include <string>

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

std::string getTextAssetFileName(const std::string& name);
std::string getMapAssetFileName(const std::string& name);
