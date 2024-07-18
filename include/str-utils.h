#pragma once

#include <pre.h>

/**
 * wstring 转 string
 * @param wstr 宽字符串
 * @return 字符串
 *
 * @author YJL
 */
inline std::string str2string(const std::wstring wstr) {
    std::string result;
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    char *buffer = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}

inline std::wstring string2str(const std::string &str) {
    std::wstring result;
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
    auto *buffer = new wchar_t[len + 1];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}


inline std::u8string str2u8str(const std::wstring &str) {
    auto src = reinterpret_cast<const char16_t *>(str.c_str());
    int32_t len = str.size() * 4;
    UErrorCode err = U_ZERO_ERROR;
    auto buffer = new char8_t[len + 1];
    u_strToUTF8(reinterpret_cast<char *>(buffer), len, &len, src, -1, &err);
    buffer[len] = '\0';
    auto r = std::u8string(buffer);
    delete[] buffer;
    return r;
}

inline std::wstring u8str2str(const std::u8string &str) {
    auto src = reinterpret_cast<const char *>(str.c_str());
    int32_t len = str.size() * 2;
    UErrorCode err = U_ZERO_ERROR;
    auto buffer = new wchar_t[len + 1];
    u_strFromUTF8(reinterpret_cast<char16_t *>(buffer), len, &len, src, -1, &err);
    buffer[len] = '\0';
    auto r = std::wstring(buffer);
    delete[] buffer;
    return r;
}

inline std::string u8str2string(const std::u8string &str) {
    return str2string(u8str2str(str));
}


inline std::u8string string2u8string(const std::string &str) {
    return str2u8str(string2str(str));
}
