#pragma once

#include <def.h>
#include <dll.h>

/**
 * wstring 转 string
 * @param wstr 宽字符串
 * @return 字符串
 *
 * @author YJL
 */
HWPP_API std::string str2string(const str &str);

HWPP_API std::wstring string2str(const std::string &str);


HWPP_API std::u8string str2u8str(const str &str);

HWPP_API std::wstring u8str2str(const u8str &str);


HWPP_API std::string u8str2string(const u8str &str);

HWPP_API std::u8string string2u8string(const std::string &str);