#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <windows.h>
#include <stdexcept>
#include <exception>
#include <commctrl.h>

#include "str-utils.h"

#ifdef min
#undef min
#define Min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifdef max
#undef max
#define Max(a, b) ((a) > (b) ? (a) : (b))
#endif

#define MAX_FRAMES 2

#define HW_FM_VIDEO L"h-wallpaper-video"

#define WM_APP_VIDEO_FILE (WM_APP+1)
#define WM_APP_QUERY_MAXIMIZED (WM_APP+2)

#define GET_CSTR(id) GetStr(id).c_str()

/**
 * @brief 显示错误并抛出异常
 * @param msg 消息
 *
 * @author YJL
 */
inline void error(LPCSTR msg) {
    MessageBoxExA(nullptr, msg, "Error", MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY, 0);
    throw std::runtime_error(msg);
}

/**
 * @brief 显示错误并抛出异常
 * @param msg 消息
 *
 * @author YJL
 */
inline void error(const std::string &msg) {
    MessageBoxExA(nullptr, msg.c_str(), "Error", MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY, 0);
    throw std::runtime_error(msg);
}

/**
 * @brief 显示错误
 * @param msg 消息
 *
 * @author YJL
 */
inline void error_not_throw(LPCSTR msg) {
    MessageBoxExA(nullptr, msg, "Error", MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY, 0);
}

/**
 * @brief 显示错误
 * @param msg 消息
 *
 * @author YJL
 */
inline void error_not_throw(const std::string &msg) {
    MessageBoxExA(nullptr, msg.c_str(), "Error", MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY, 0);
}
