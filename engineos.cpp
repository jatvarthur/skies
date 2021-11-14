#include "engine.h"
#include <sstream>
#include <cstdarg>
#include "winos.h"


void _debug(const wchar_t* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	wchar_t s[255] = { 0 };
	_vsnwprintf_s(s, 254, fmt, args);
	OutputDebugString(s);
	va_end(args);
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
