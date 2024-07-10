#include <pre.h>

#include "wallpaper-window.h"
#include "config.h"

using namespace std;

HANDLE hMutex;

string appPath;

string exePath;
wstring exeWPath;

void closeMutex() {
    CloseHandle(hMutex);
}

extern WallpaperWindow *wallpaperWindow;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

    int argc = 0;
    LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &argc);
    exeWPath = args[0];
    exePath = wstring2string(args[0]);
    appPath = wstring2string(args[0]);
    appPath = appPath.substr(0, appPath.find_last_of("\\/"));

    hMutex = CreateMutex(nullptr, FALSE, APP_NAME);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        HWND hWnd = WallpaperWindow::FindExist();
        if (argc > 1) {
            PostMessageW(hWnd, WM_APP_VIDEO_FILE, 0, 0);
            HANDLE hMapFile;
            char *pData;

            // 打开共享内存
            hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, HW_FM_VIDEO);
            if (!hMapFile) {
                error_format_not_throw("OpenFileMapping failed with error code %d");
                goto end;
            }

            // 映射共享内存到进程地址空间
            pData = (char *) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
            if (!pData) {
                error_format_not_throw("MapViewOfFile failed with error code %d");
                goto end;
            }

            auto fileName = wstring2string(args[1]);
            strcpy_s(pData, fileName.length() + 1, fileName.c_str());

            UnmapViewOfFile(pData);
            CloseHandle(hMapFile);
        } else
            MessageBox(nullptr, "H-Wallpaper is already running.", "Error", MB_OK | MB_ICONERROR);
        end:
        return ERROR_ALREADY_EXISTS;
    }
    atexit(closeMutex);

    initConfig();
    wallpaperWindow = new WallpaperWindow(hInstance);

    wallpaperWindow->SetToDesktop();
    wallpaperWindow->Show();

    if (argc > 1)
        wallpaperWindow->SetVideo(wstring2string(args[1]));
    else {
        if (config["wallpaper"].IsScalar())
            wallpaperWindow->SetVideo(config["wallpaper"].as<string>(), false);
    }

    std::atomic<bool> queryRun = true;

    thread queryMaximizedThread([&]() {
        while (queryRun.load()) {
            Sleep(500);
            wallpaperWindow->PostQueryMaximized();
        }
    });

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    queryRun.store(false);
    queryMaximizedThread.join();
    delete wallpaperWindow;
    return ERROR_SUCCESS;
}

