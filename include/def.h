#pragma once

#include <string>

#ifdef UNICODE
typedef std::wstring str;
#else
typedef std::string str;
#endif

typedef std::u8string u8str;
