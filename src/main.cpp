#include <iostream>
#include <windows.h>

#include "wallpaper-window.h"
#include "str-utils.h"

using namespace std;

WallpaperWindow *wallpaperWindow;

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

