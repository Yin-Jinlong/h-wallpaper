#include <pre.h>

#include "wallpaper-window.h"
#include "str-utils.h"

using namespace std;


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

    int argc = 0;
    LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &argc);

    auto wallpaperWindow = new WallpaperWindow(hInstance);

    wallpaperWindow->SetToDesktop();
    wallpaperWindow->Show();

    if (argc > 1)
        wallpaperWindow->SetVideo(wstring2string(args[1]));

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    delete wallpaperWindow;
    return ERROR_SUCCESS;
}

