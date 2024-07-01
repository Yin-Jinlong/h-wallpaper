#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <windows.h>

inline void error(WCHAR *msg) {
    MessageBoxExW(NULL, msg, L"Error", MB_OK | MB_ICONERROR, 0);
    ExitProcess(1);
}

inline void error(const WCHAR *msg) {
    MessageBoxExW(NULL, msg, L"Error", MB_OK | MB_ICONERROR, 0);
    ExitProcess(1);
}
