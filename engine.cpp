#include "engine.h"
#include <sstream>


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
