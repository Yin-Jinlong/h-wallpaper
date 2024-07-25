#include <pre.h>

#include "sys-err.h"
#include "wallpaper-window.h"
#include "wallpapers/video-wallpaper.h"

using namespace std;

HANDLE hMutex;

string appPath;

str exeWPath;

void onexit() {
    CloseHandle(hMutex);
    SaveConfig();
}

extern WallpaperWindow *wallpaperWindow;

int sendVideoFile(const str &file) {
    HWND hWnd = WallpaperWindow::FindExist();
    PostMessage(hWnd, WM_APP_VIDEO_FILE, 0, 0);
    HANDLE hMapFile;
    char *pData;

    // 打开共享内存
    hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, HW_FM_VIDEO);
    if (!hMapFile) {
        auto err = GetLastError();
        error_not_throw(format(TEXT("OpenFileMapping failed with error code {}"), err));
        return err;
    }

    // 映射共享内存到进程地址空间
    pData = (char *) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!pData) {
        auto err = GetLastError();
        error_not_throw(format(TEXT("MapViewOfFile failed with error code {}"), err));
        return err;
    }

    auto u8String = str2u8str(file);
    auto len = strlen(reinterpret_cast<const char *>(u8String.c_str()));
    memcpy(pData, &len, 2);
    memcpy(pData + 2, u8String.c_str(), len);

    UnmapViewOfFile(pData);
    CloseHandle(hMapFile);
    return 0;
}

void checkUpdateOnStart() {
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi;
    auto cmd = std::format(TEXT("{}-updater --only-new"), APP_NAME);
    if (!CreateProcess(nullptr,
                       cmd.data(),
                       nullptr, nullptr,
                       TRUE, 0,
                       nullptr, nullptr,
                       &si, &pi)) {
        error_message(GetLastError());
        return;
    }
    CloseHandle(pi.hThread);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

    int argc = 0;
    LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &argc);
    exeWPath = args[0];
    appPath = str2string(args[0]);
    appPath = appPath.substr(0, appPath.find_last_of("\\/"));

    hMutex = CreateMutex(nullptr, FALSE, APP_NAME);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        if (argc > 1)
            return sendVideoFile(args[1]);
        MessageBox(nullptr, TEXT("H-Wallpaper is already running."), TEXT("Error"), MB_OK | MB_ICONERROR);
        return ERROR_ALREADY_EXISTS;
    }
    atexit(onexit);

    initConfig();
    wallpaperWindow = new WallpaperWindow(hInstance);

    wallpaperWindow->SetToDesktop();
    wallpaperWindow->Show();

    auto wpp = (VideoWallpaper *) wallpaperWindow->GetWallpaper();
    if (argc > 1) {
        if (wpp->SetVideo(args[1])) {
            SaveConfig();
        }
    } else if (!config.wallpaper.file.empty()) {
        wpp->SetVideo(u8str2str(config.wallpaper.file), config.wallpaper.time);
    }

    std::atomic<bool> queryRun = true;

    thread queryMaximizedThread([&]() {
        while (queryRun.load()) {
            Sleep(500);
            wallpaperWindow->PostQueryMaximized();
        }
    });

    if (config.update.checkOnStart) {
        checkUpdateOnStart();
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    queryRun.store(false);
    queryMaximizedThread.join();
    delete wallpaperWindow;
    return ERROR_SUCCESS;
}
