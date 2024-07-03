#include <pre.h>

#include "wallpaper-window.h"
#include "str-utils.h"

using namespace std;

HANDLE hMutex;

void closeMutex() {
    CloseHandle(hMutex);
}


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

    int argc = 0;
    LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &argc);

    hMutex = CreateMutexW(nullptr, FALSE, L"H-Wallpaper");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        HWND hWnd = WallpaperWindow::FindExist();
        if (argc > 1) {
            PostMessageW(hWnd, WM_APP_VIDEO_FILE, 0, 0);
            HANDLE hMapFile;
            char *pData;

            // 打开共享内存
            hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, HW_FM_VIDEO);
            if (!hMapFile) {
                error_format_not_throw(L"OpenFileMapping failed with error code %d");
                goto end;
            }

            // 映射共享内存到进程地址空间
            pData = (char *) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
            if (!pData) {
                error_format_not_throw(L"MapViewOfFile failed with error code %d");
                goto end;
            }

            auto fileName = wstring2string(args[1]);
            strcpy_s(pData, fileName.length() + 1, fileName.c_str());

            UnmapViewOfFile(pData);
            CloseHandle(hMapFile);
        } else
            MessageBoxW(nullptr, L"H-Wallpaper is already running.", L"Error", MB_OK | MB_ICONERROR);
        end:
        return ERROR_ALREADY_EXISTS;
    }
    atexit(closeMutex);

    auto wallpaperWindow = new WallpaperWindow(hInstance);

    wallpaperWindow->SetToDesktop();
    wallpaperWindow->Show();

    if (argc > 1)
        wallpaperWindow->SetVideo(wstring2string(args[1]));

    thread queryMaxSizeThread([&]() {
        while (true) {
            Sleep(500);
            wallpaperWindow->PostQueryMaximized();
        }
    });

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    delete wallpaperWindow;
    TerminateThread(queryMaxSizeThread.native_handle(), 0);
    return ERROR_SUCCESS;
}

