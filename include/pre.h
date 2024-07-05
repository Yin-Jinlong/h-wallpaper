#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <windows.h>
#include <stdexcept>
#include <exception>

#include "str-utils.h"

#include "resources.h"

#define MAX_FRAMES 2

#define HW_FM_VIDEO "h-wallpaper-video"

#define WM_APP_VIDEO_FILE (WM_APP+1)
#define WM_APP_QUERY_MAXIMIZED (WM_APP+2)

inline void error(CHAR *msg) {
    MessageBoxEx(nullptr, msg, "Error", MB_OK | MB_ICONERROR, 0);
    throw std::runtime_error(msg);
}

inline void error(const CHAR *msg) {
    MessageBoxEx(nullptr, msg, "Error", MB_OK | MB_ICONERROR, 0);
    throw std::runtime_error(msg);
}

inline void error_format_not_throw(const CHAR *msg, ...) {
    CHAR buf[1024];
    sprintf_s(buf, 1024, msg);
    MessageBoxEx(nullptr, buf, "Error", MB_OK | MB_ICONERROR, 0);
}
