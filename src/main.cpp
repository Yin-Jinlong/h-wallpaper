#include <iostream>
#include <windows.h>
#include <tchar.h>

#include "wallpaper-window.h"

using namespace std;

WallpaperWindow *wallpaperWindow;

string wstring2string(const wstring &wstr) {
    string result;
    //获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    char *buffer = new char[len + 1];
    //宽字节编码转换成多字节编码
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    //删除缓冲区并返回值
    result.append(buffer);
    delete[] buffer;
    return result;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

    int argc = 0;
    LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &argc);

    wallpaperWindow = new WallpaperWindow(hInstance);

    wallpaperWindow->SetToDesktop();
    wallpaperWindow->Show();

    MSG msg;

    if (argc > 1)
        wallpaperWindow->SetVideo(wstring2string(args[1]));

    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    delete wallpaperWindow;
    return ERROR_SUCCESS;
}

