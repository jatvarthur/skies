#include "..\engine.h"
#include <sstream>
#include <cstdarg>
#include "..\winos.h"

#undef max
#undef min

void _debug(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char s[255] = { 0 };
	_vsnprintf_s(s, 254, fmt, args);
	OutputDebugStringA(s);
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


void loadString(std::istream& is, std::string& str, bool fromNewLine)
{
	str.clear();
	if (fromNewLine) is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::getline(is, str, '\n');
	ltrim(str);
}

void loadText(std::istream& is, std::vector<std::string>& text)
{
	int nStrings;
	is >> nStrings;
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	text.reserve(nStrings);
	for (int i = 0; i < nStrings; ++i) {
		std::string s;
		std::getline(is, s, '\n');
		text.push_back(std::move(s));
	}
}
