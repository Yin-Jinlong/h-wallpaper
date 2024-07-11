#include <wallpaper-window.h>
#include <ShObjIdl_core.h>
#include <shlobj.h>

#include "resources.h"
#include "wnd-utils.h"
#include "config.h"
#include "file-utils.h"
#include "string-table.h"

#define PMID_EXIT 1
#define PMID_CHANGE 2
#define PMID_RUN_ON_STARTUP 3

#define REG_RUN_KEY TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run")

static const WCHAR *HWallpaperWindowClassName = TEXT("YJL-WALLPAPER");

extern std::string appPath;
extern std::string exePath;
extern std::wstring exeWPath;

WallpaperWindow *wallpaperWindow;

LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void RegisterWndClass(HINSTANCE hInstance) {
    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(wndClass);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WindowProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = ::LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
    wndClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH) ::GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = HWallpaperWindowClassName;
    wndClass.hIconSm = nullptr;

    if (!RegisterClassEx(&wndClass)) {
        error("RegisterClassEx failed");
    }
}

WallpaperWindow::WallpaperWindow(HINSTANCE hInstance) {
    RegisterWndClass(hInstance);
    wallpaperWindow = this;

    hWnd = CreateWindowEx(0,
                          HWallpaperWindowClassName, TEXT("YJL_WALLPAPER"),
                          WS_POPUP | WS_VISIBLE | WS_MAXIMIZE,
                          0, 0,
                          GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                          nullptr, nullptr,
                          hInstance,
                          nullptr);

    if (hWnd == nullptr) {
        error("CreateWindowEx failed");
    }

    avformat_network_init();
}

void WallpaperWindow::Show() {
    ShowWindow(hWnd, SW_MAXIMIZE);
    UpdateWindow(hWnd);
}

HWND PMWindow() {
    return FindWindow(TEXT("Progman"), TEXT("Program Manager"));
}

HWND splitDesktopWindow() {
    HWND hWnd = PMWindow();
    if (hWnd != nullptr) {
        SendMessage(hWnd, 0x052C, 0, 0);
    }
    return hWnd;
}

BOOL CALLBACK CloseWorker2(HWND tophandle, LPARAM _) {
    HWND defview = FindWindowEx(tophandle, nullptr, TEXT("SHELLDLL_DefView"), nullptr);
    if (defview != nullptr) {
        ShowWindow(FindWindowEx(nullptr, tophandle, TEXT("WorkerW"), nullptr), SW_HIDE);
    }
    return true;
}

HWND WallpaperWindow::FindExist() {
    HWND hwnd = FindWindowEx(PMWindow(), nullptr, HWallpaperWindowClassName, nullptr);
    return hwnd;
}

void WallpaperWindow::SetToDesktop() {
    HWND desktop = splitDesktopWindow();
    EnumWindows(CloseWorker2, 0);
    SetParent(hWnd, desktop);
}

void WallpaperWindow::SetVideo(std::string file, bool save) {
    if (file.empty())
        return;
    VideoDecoder *nd;
    try {
        nd = new VideoDecoder(file);
    } catch (...) {
        return;
    }
    {
        auto decoder = decoderPtr.load();
        if (decoder) {
            decoder->close();
            delete decoder;
            decoderPtr.store(nullptr);
        }
    }
    nowTime = 0;
    frameTime = 0;
    lastTime = 0;
    //**************************//
    // 到此步屏幕（窗口）尺寸已经有了 //
    // 前面WM_SIZE已经处理过了     //
    //**************************//
    nd->width = width;
    nd->height = height;
    decoderPtr.store(nd);
    if (save) {
        config["wallpaper"] = file;
        SaveConfig();
    }
    nd->startDecode();
    InvalidateRect(hWnd, nullptr, false);
}

WallpaperWindow::~WallpaperWindow() {
    auto decoder = decoderPtr.load();
    if (decoder) {
        decoder->close();
        delete decoder;
        decoderPtr.store(nullptr);
    }
}

void WallpaperWindow::paint(HDC hdc) {
    auto decoder = decoderPtr.load();
    if (!decoder || frameTime > nowTime)
        return;

    auto vf = decoder->getFrame();
    if (!vf)
        return;
    HDC mdc = CreateCompatibleDC(hdc);

    HBITMAP hBitmap = vf->bitmap;

    auto dt = av_q2d(decoder->time_base) * vf->duration;
    frameTime += dt;

    SelectObject(mdc, hBitmap);

    // 缩放已经处理，用BitBlt更高效
    //StretchBlt(hdc, 0, 0, width, height,
    //           mdc, 0, 0, vf->width, vf->height,
    //           SRCCOPY
    //);
    BitBlt(hdc, 0, 0, vf->width, vf->height, mdc, 0, 0, SRCCOPY);
    DeleteObject(hBitmap);
    DeleteDC(mdc);
}

bool WallpaperWindow::decoderAvailable() {
    auto decoder = decoderPtr.load();
    return decoder && decoder->running();
}

bool WallpaperWindow::firstFrameLoaded() {
    auto decoder = decoderPtr.load();
    return decoder && decoder->firstFrameLoaded();
}

void WallpaperWindow::SetSize(int width, int height) {
    this->width = width;
    this->height = height;
}

bool WallpaperWindow::decoderPaused() {
    auto decoder = decoderPtr.load();
    return !decoder || decoder->paused();
}

void WallpaperWindow::pause() {
    auto decoder = decoderPtr.load();
    if (decoder) {
        decoder->pause();
    }
}

void WallpaperWindow::resume() {
    auto decoder = decoderPtr.load();
    if (decoder) {
        decoder->resume();
    }
    InvalidateRect(hWnd, nullptr, false);
}

void WallpaperWindow::PostQueryMaximized() {
    PostMessageW(hWnd, WM_APP_QUERY_MAXIMIZED, 0, 0);
}

int WallpaperWindow::GetWidth() const {
    return width;
}

int WallpaperWindow::GetHeight() const {
    return height;
}

DEVMODE dm;
HMENU trayMenu;
NOTIFYICONDATA nid;
HANDLE hMapFile = nullptr;

double toTime(SYSTEMTIME t) {
    return (t.wHour * 3600 + t.wMinute * 60 + t.wSecond) + t.wMilliseconds / 1000.0;
}

void CreateMapping() {
    hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            nullptr,
            PAGE_READWRITE,
            0,
            sizeof(double),
            HW_FM_VIDEO);
    if (hMapFile == nullptr) {
        error("CreateFileMappingW failed");
    }
}

/**
 * 绘制
 * @param hdc
 * @return 是否重绘
 */
bool StartPaint(HDC hdc) {
    // 没有视频，绘制黑色
    if (!wallpaperWindow->decoderAvailable()) {
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
        RECT rect = {0};
        rect.right = wallpaperWindow->GetWidth();
        rect.bottom = wallpaperWindow->GetHeight();
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
        return false;
    }

    // 暂停了
    if (wallpaperWindow->decoderPaused()) {
        return false;
    }

    // 还没加载第一帧
    if (!wallpaperWindow->firstFrameLoaded()) {
        return true;
    }

    SYSTEMTIME now;
    GetSystemTime(&now);
    double dt = toTime(now) - wallpaperWindow->lastTime;
    if (wallpaperWindow->lastTime == 0) {
        dt = 1.0 / dm.dmDisplayFrequency;
    }
    wallpaperWindow->lastTime = toTime(now);
    wallpaperWindow->nowTime += dt;
    wallpaperWindow->paint(hdc);
    return true;
}

std::wstring GetStartupPath() {
    HRESULT hr;
    PWSTR pszPath = nullptr;

    hr = SHGetKnownFolderPath(FOLDERID_Startup, 0, nullptr, &pszPath);
    if (SUCCEEDED(hr)) {
        std::wstring path = pszPath;
        CoTaskMemFree(pszPath);
        return path;
    }
    error("GetStartupPath failed");
    return L"";
}

bool RegHasValue(HKEY hkey, LPCWSTR subKey) {
    return RegQueryValue(hkey, subKey, nullptr, nullptr) == ERROR_SUCCESS;
}

bool isRunOnStartup() {
    DWORD size = 260;
    WCHAR value[260];
    DWORD err;
    if ((err = RegGetValue(HKEY_CURRENT_USER, REG_RUN_KEY, APP_NAME,
                           RRF_RT_REG_SZ, nullptr, value, &size))) {
        if (err == ERROR_FILE_NOT_FOUND) {
            return false;
        }
        error("RegQueryValueEx failed");
    }
    return exeWPath == value;
}

void setRunOnStartup(bool run) {
    HKEY runKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_RUN_KEY, 0, KEY_WRITE, &runKey)) {
        error("RegOpenKeyEx failed");
    }
    if (run) {
        if (RegSetValueEx(runKey, APP_NAME, 0, REG_SZ,
                          (BYTE *) exeWPath.c_str(),
                          static_cast<DWORD>(exeWPath.size() * sizeof(WCHAR)))) {
            RegCloseKey(runKey);
            error("RegSetValueEx failed");
        }
    } else {
        if (RegHasValue(runKey, APP_NAME)) {
            if (RegDeleteValue(runKey, APP_NAME)) {
                RegCloseKey(runKey);
                error("RegDeleteValue failed");
            }
        }
    }
    RegCloseKey(runKey);
}

LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            dm.dmSize = sizeof(DEVMODE);
            EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dm);

            nid.cbSize = sizeof(nid);
            nid.hWnd = hWnd;
            nid.uID = 0;
            nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            nid.uCallbackMessage = WM_USER;
            nid.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
            lstrcpy(nid.szTip, APP_NAME);
            Shell_NotifyIcon(NIM_ADD, &nid);

            trayMenu = CreatePopupMenu();


            AppendMenu(trayMenu,
                       isRunOnStartup() ? MF_CHECKED : MF_UNCHECKED, PMID_RUN_ON_STARTUP,
                       GetStr(IDS_RUN_ON_STARTUP).c_str());
            AppendMenu(trayMenu, MF_STRING, PMID_CHANGE, GetStr(IDS_CHOOSE_FILE).c_str());
            AppendMenu(trayMenu, MF_SEPARATOR, 0, nullptr);
            AppendMenu(trayMenu, MF_STRING, PMID_EXIT, GetStr(IDS_EXIT).c_str());
            Shell_NotifyIcon(NIM_ADD, &nid);

            CreateMapping();
            break;
        }
        case WM_SIZE:
            //*********************************//
            // 在设置系统缩放后，创建窗口时会调用两次 //
            // 第二次的尺寸才是正确的              //
            //********************************//
            wallpaperWindow->SetSize(LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (StartPaint(hdc)) {
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            EndPaint(hWnd, &ps);
            break;
        }
        case WM_USER: {
            POINT pt;
            if (lParam == WM_RBUTTONDOWN) {
                GetCursorPos(&pt);
                SetForegroundWindow(hWnd);
                CheckMenuItem(trayMenu, PMID_RUN_ON_STARTUP, isRunOnStartup() ? MF_CHECKED : MF_UNCHECKED);
                int id = TrackPopupMenuEx(trayMenu, TPM_RETURNCMD, pt.x, pt.y, hWnd, nullptr);
                switch (id) {
                    case PMID_EXIT:
                        DestroyWindow(hWnd);
                        break;
                    case PMID_CHANGE: {

                        OPENFILENAME ofn;
                        WCHAR szFile[260];

                        ZeroMemory(&ofn, sizeof(ofn));
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = hWnd;
                        ofn.lpstrFile = szFile;
                        ofn.lpstrFile[0] = '\0';
                        ofn.nMaxFile = sizeof(szFile);
                        ofn.lpstrFilter = TEXT("video(*.mp4)\0*.mp4\0\0");
                        ofn.nFilterIndex = 1;
                        ofn.lpstrFileTitle = nullptr;
                        ofn.nMaxFileTitle = 0;
                        ofn.lpstrInitialDir = nullptr;
                        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                        if (GetOpenFileName(&ofn)) {
                            wallpaperWindow->SetVideo(wstring2string(ofn.lpstrFile));
                            InvalidateRect(hWnd, nullptr, FALSE);
                        }
                        break;
                    }
                    case PMID_RUN_ON_STARTUP: {
                        setRunOnStartup(!isRunOnStartup());
                        break;
                    }
                }
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            CloseHandle(hMapFile);
            Shell_NotifyIcon(NIM_DELETE, &nid);
            break;
        case WM_APP_QUERY_MAXIMIZED:
            if (HasWindowMaximized()) {
                if (!wallpaperWindow->decoderPaused())
                    wallpaperWindow->pause();
            } else {
                if (wallpaperWindow->decoderPaused()) {
                    wallpaperWindow->lastTime = 0;
                    wallpaperWindow->resume();
                }
            }
            break;
        case WM_APP_VIDEO_FILE: {
            char *pData = (char *) MapViewOfFile(
                    hMapFile,
                    FILE_MAP_READ, 0, 0, 0
            );
            if (pData) {
                std::string file;
                file += pData;
                UnmapViewOfFile(pData);
                wallpaperWindow->SetVideo(file);
            }
            break;
        }
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

#undef REG_RUN_KEY
