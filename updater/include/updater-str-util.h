#pragma once

#include <pre.h>

inline std::string u8AsString(u8str u8) {
    return std::string(u8.begin(), u8.end());
}

inline u8str stringAsU8str(const std::string &s) {
    return u8str(s.begin(), s.end());
}
