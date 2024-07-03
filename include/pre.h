#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <windows.h>
#include <stdexcept>
#include <exception>

#include "str-utils.h"

#include "resources.h"

#define HW_FM_VIDEO L"h-wallpaper-video"

#define WM_APP_VIDEO_FILE (WM_APP+1)

inline void error(WCHAR *msg) {
    MessageBoxExW(NULL, msg, L"Error", MB_OK | MB_ICONERROR, 0);
    throw std::runtime_error(wstring2string(msg));
}

inline void error(const WCHAR *msg) {
    MessageBoxExW(NULL, msg, L"Error", MB_OK | MB_ICONERROR, 0);
    throw std::runtime_error(wstring2string(msg));
}

inline void error_format_not_throw(const WCHAR *msg, ...) {
    WCHAR buf[1024];
    swprintf_s(buf, 1024, msg);
    MessageBoxExW(NULL, buf, L"Error", MB_OK | MB_ICONERROR, 0);
}
