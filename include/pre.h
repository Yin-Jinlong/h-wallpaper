#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <windows.h>
#include <stdexcept>
#include <exception>
#include <commctrl.h>

#include "str-utils.h"

#define MAX_FRAMES 2

#define HW_FM_VIDEO L"h-wallpaper-video"

#define WM_APP_VIDEO_FILE (WM_APP+1)
#define WM_APP_QUERY_MAXIMIZED (WM_APP+2)

/**
 * @brief 显示错误并抛出异常
 * @param msg 消息
 *
 * @author YJL
 */
inline void error(CHAR *msg) {
    MessageBoxExA(nullptr, msg, "Error", MB_OK | MB_ICONERROR, 0);
    throw std::runtime_error(msg);
}

/**
 * @brief 显示错误并抛出异常
 * @param msg 消息
 *
 * @author YJL
 */
inline void error(const CHAR *msg) {
    MessageBoxExA(nullptr, msg, "Error", MB_OK | MB_ICONERROR, 0);
    throw std::runtime_error(msg);
}

/**
 * @brief 显示错误并格式化
 * @param msg 消息
 *
 * @author YJL
 */
inline void error_format_not_throw(const CHAR *msg, ...) {
    CHAR buf[1024];
    sprintf_s(buf, 1024, msg);
    MessageBoxExA(nullptr, buf, "Error", MB_OK | MB_ICONERROR, 0);
}
