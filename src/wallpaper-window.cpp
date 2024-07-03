#include <wallpaper-window.h>

#include "str-utils.h"

#define PMID_EXIT 1
#define PMID_CHANGE 2

static const wchar_t *HWallpaperWindowClassName = L"YJL-WALLPAPER";

WallpaperWindow *wallpaperWindow;

LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void RegisterWndClass(HINSTANCE hInstance) {
    WNDCLASSEXW wndClass;
    wndClass.cbSize = sizeof(wndClass);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WindowProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = ::LoadIcon(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDI_ICON1));
    wndClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH) ::GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = HWallpaperWindowClassName;
    wndClass.hIconSm = nullptr;

    if (!RegisterClassExW(&wndClass)) {
        error(L"RegisterClassExW failed");
    }
}

WallpaperWindow::WallpaperWindow(HINSTANCE hInstance) {
    RegisterWndClass(hInstance);
    wallpaperWindow = this;

    hWnd = CreateWindowExW(0,
                           HWallpaperWindowClassName, L"YJL_WALLPAPER",
                           WS_POPUP | WS_VISIBLE | WS_MAXIMIZE,
                           0, 0,
                           GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                           nullptr, nullptr,
                           hInstance,
                           nullptr);

    if (hWnd == nullptr) {
        error(L"CreateWindowExW failed");
    }

    avformat_network_init();
}

void WallpaperWindow::Show() {
    ShowWindow(hWnd, SW_MAXIMIZE);
    UpdateWindow(hWnd);
}

HWND PMWindow() {
    return FindWindowW(L"Progman", L"Program Manager");
}

HWND splitDesktopWindow() {
    HWND hWnd = PMWindow();
    if (hWnd != nullptr) {
        SendMessageW(hWnd, 0x052C, 0, 0);
    }
    return hWnd;
}

BOOL CALLBACK CloseWorker2(HWND tophandle, LPARAM _) {
    HWND defview = FindWindowExW(tophandle, nullptr, L"SHELLDLL_DefView", nullptr);
    if (defview != nullptr) {
        ShowWindow(FindWindowExW(nullptr, tophandle, L"WorkerW", nullptr), SW_HIDE);
    }
    return true;
}

HWND WallpaperWindow::FindExist() {
    HWND hwnd = FindWindowExW(PMWindow(), nullptr, HWallpaperWindowClassName, nullptr);
    return hwnd;
}

void WallpaperWindow::SetToDesktop() {
    HWND desktop = splitDesktopWindow();
    EnumWindows(CloseWorker2, 0);
    SetParent(hWnd, desktop);
}

void WallpaperWindow::SetVideo(std::string file) {
    auto decoder = decoderPtr.load();
    if (decoder) {
        decoder->close();
        delete decoder;
        decoderPtr.store(nullptr);
    }
    nowTime = 0;
    frameTime = 0;
    lastTime = 0;
    try {
        decoder = new VideoDecoder(file);
        //**************************//
        // 到此步屏幕（窗口）尺寸已经有了 //
        // 前面WM_SIZE已经处理过了     //
        //**************************//
        decoder->width = width;
        decoder->height = height;
        decoderPtr.store(decoder);
        decoder->startDecode();
    } catch (std::exception &e) {
        decoderPtr.store(nullptr);
    }
    InvalidateRect(hWnd, nullptr, false);
}

WallpaperWindow::~WallpaperWindow() = default;

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

DEVMODE dm;
HMENU trayMenu;
NOTIFYICONDATA nid;
HANDLE hMapFile = nullptr;

double toTime(SYSTEMTIME t) {
    return (t.wHour * 3600 + t.wMinute * 60 + t.wSecond) + t.wMilliseconds / 1000.0;
}

void CreateMapping() {
    hMapFile = CreateFileMappingW(
            INVALID_HANDLE_VALUE,
            nullptr,
            PAGE_READWRITE,
            0,
            sizeof(double),
            HW_FM_VIDEO);
    if (hMapFile == nullptr) {
        error(L"CreateFileMappingW failed");
    }
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
            nid.hIcon = LoadIcon(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDI_ICON1));
            lstrcpy(nid.szTip, "h-wallpaper");
            Shell_NotifyIcon(NIM_ADD, &nid);

            trayMenu = CreatePopupMenu();
            AppendMenuW(trayMenu, MF_STRING, PMID_CHANGE, L"选择文件");
            AppendMenuW(trayMenu, MF_STRING, PMID_EXIT, L"退出");
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
            if (!wallpaperWindow->decoderAvailable()) {
                HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
                FillRect(hdc, &ps.rcPaint, hBrush);
                DeleteObject(hBrush);
                EndPaint(hWnd, &ps);
                break;
            }

            InvalidateRect(hWnd, nullptr, FALSE);

            if (!wallpaperWindow->firstFrameLoaded()) {
                EndPaint(hWnd, &ps);
                break;
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
            EndPaint(hWnd, &ps);
            break;
        }
        case WM_USER: {
            POINT pt;
            if (lParam == WM_RBUTTONDOWN) {
                GetCursorPos(&pt);
                SetForegroundWindow(hWnd);
                int id = TrackPopupMenuEx(trayMenu, TPM_RETURNCMD, pt.x, pt.y, hWnd, nullptr);
                switch (id) {
                    case PMID_EXIT:
                        PostQuitMessage(0);
                        break;
                    case PMID_CHANGE: {

                        OPENFILENAMEW ofn;
                        WCHAR szFile[260];

                        ZeroMemory(&ofn, sizeof(ofn));
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = hWnd;
                        ofn.lpstrFile = szFile;
                        ofn.lpstrFile[0] = '\0';
                        ofn.nMaxFile = sizeof(szFile);
                        ofn.lpstrFilter = L"video(*.mp4)\0*.mp4\0\0";
                        ofn.nFilterIndex = 1;
                        ofn.lpstrFileTitle = nullptr;
                        ofn.nMaxFileTitle = 0;
                        ofn.lpstrInitialDir = nullptr;
                        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                        if (GetOpenFileNameW(&ofn)) {
                            wallpaperWindow->SetVideo(wstring2string(ofn.lpstrFile));
                            InvalidateRect(hWnd, nullptr, FALSE);
                        }
                        break;
                    }
                }
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            Shell_NotifyIcon(NIM_DELETE, &nid);
            break;
        case WM_QUIT:
            CloseHandle(hMapFile);
            break;
        case WM_APP_VIDEO_FILE: {
            char *pData = (char *) MapViewOfFile(
                    hMapFile,
                    FILE_MAP_READ, 0, 0, 0
            );
            if (pData) {
                std::string file;
                file+=pData;
                UnmapViewOfFile(pData);
                wallpaperWindow->SetVideo(file);
            }
            break;
        }
        default:
            return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
}
