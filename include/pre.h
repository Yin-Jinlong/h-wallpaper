#pragma once

#include <pre-config.h>

#include <windows.h>

#include <commctrl.h>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <icu.h>
#include <stdexcept>
#include <string>

// windows 宏 与部分c++用法冲突

#ifdef min
    #undef min
    #define Min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifdef max
    #undef max
    #define Max(a, b) ((a) > (b) ? (a) : (b))
#endif

// 声明

// 常值

#define MAX_FRAMES 2

#define HW_FM_VIDEO L"h-wallpaper-video"

// 替换

#define USE_RET [[nodiscard]]

#define WM_APP_VIDEO_FILE (WM_APP + 1)
#define WM_APP_QUERY_MAXIMIZED (WM_APP + 2)

#define GET_CSTR(id) GetStr(id).c_str()


#include "str-utils.h"
#include <def.h>

/**
 * @brief 显示错误并抛出异常
 * @param msg 消息
 *
 * @author YJL
 */
inline void error(const str &msg) {
    MessageBoxEx(nullptr, msg.c_str(), TEXT("Error"), MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY, 0);
    throw std::runtime_error(str2string(msg));
}

/**
 * @brief 显示错误
 * @param msg 消息
 *
 * @author YJL
 */
inline void error_not_throw(LPCWSTR msg) {
    MessageBoxEx(nullptr, msg, TEXT("Error"), MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY, 0);
}

/**
 * @brief 显示错误
 * @param msg 消息
 *
 * @author YJL
 */
inline void error_not_throw(const std::wstring &msg) {
    MessageBoxEx(nullptr, msg.c_str(), TEXT("Error"), MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY, 0);
}
