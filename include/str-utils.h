#pragma once

#include <pre.h>

/**
 * wstring 转 string
 * @param wstr 宽字符串
 * @return 字符串
 *
 * @author YJL
 */
inline std::string wstring2string(const std::wstring wstr) {
    std::string result;
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    char *buffer = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}
