#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <windows.h>
#include <stdexcept>
#include <exception>

#include "str-utils.h"

inline void error(WCHAR *msg) {
    MessageBoxExW(NULL, msg, L"Error", MB_OK | MB_ICONERROR, 0);
    throw std::runtime_error(wstring2string(msg));
}

inline void error(const WCHAR *msg) {
    MessageBoxExW(NULL, msg, L"Error", MB_OK | MB_ICONERROR, 0);
    throw std::runtime_error(wstring2string(msg));
}
