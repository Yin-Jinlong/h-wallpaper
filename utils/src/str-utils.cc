#include <str-utils.h>
#include <windows.h>
#include <icu.h>

/**
 * wstring 转 string
 * @param wstr 宽字符串
 * @return 字符串
 *
 * @author YJL
 */
std::string str2string(const str &str) {
    std::string result;
    int len = WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), NULL, 0, NULL, NULL);
    char *buffer = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}

str string2str(const std::string &str) {
    std::wstring result;
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
    auto *buffer = new wchar_t[len + 1];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}


u8str str2u8str(const str &str) {
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

str u8str2str(const u8str &str) {
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

std::string u8str2string(const u8str &str) {
    return str2string(u8str2str(str));
}


u8str string2u8string(const std::string &str) {
    return str2u8str(string2str(str));
}
