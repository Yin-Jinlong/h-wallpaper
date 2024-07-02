#include <wallpaper-window.h>

#include "str-utils.h"

#define PMID_EXIT 0
#define PMID_CHANGE 1

extern WallpaperWindow *wallpaperWindow;

LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void RegisterWndClass(HINSTANCE hInstance) {
    WNDCLASSEXW wndClass;
    wndClass.cbSize = sizeof(wndClass);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WindowProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);
    wndClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH) ::GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = L"YJL-WALLPAPER";
    wndClass.hIconSm = nullptr;

    if (!RegisterClassExW(&wndClass)) {
        error(L"RegisterClassExW failed");
    }
}

WallpaperWindow::WallpaperWindow(HINSTANCE hInstance) {
    RegisterWndClass(hInstance);

    hWnd = CreateWindowExW(0,
                           L"YJL-WALLPAPER", L"YJL_WALLPAPER",
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

HWND splitDesktopWindow() {
    HWND hWnd = FindWindowW(L"Progman", L"Program Manager");
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
        decoderPtr.store(decoder);
        decoder->startDecode();
    } catch (std::exception &e) {
        decoderPtr.store(nullptr);
    }
}

WallpaperWindow::~WallpaperWindow() = default;

void WallpaperWindow::paint(HWND hWnd, HDC hdc) {
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

    RECT rect;
    GetWindowRect(hWnd, &rect);

    StretchBlt(hdc, 0, 0, rect.right, rect.bottom,
               mdc, 0, 0, vf->width, vf->height,
               SRCCOPY
    );

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

DEVMODE dm;
HMENU trayMenu;
NOTIFYICONDATA nid;

double toTime(SYSTEMTIME t) {
    return (t.wHour * 3600 + t.wMinute * 60 + t.wSecond) + t.wMilliseconds / 1000.0;
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
            nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
            lstrcpy(nid.szTip, "h-wallpaper");
            Shell_NotifyIcon(NIM_ADD, &nid);

            trayMenu = CreatePopupMenu();
            AppendMenuW(trayMenu, MF_STRING, PMID_CHANGE, L"选择文件");
            AppendMenuW(trayMenu, MF_STRING, PMID_EXIT, L"退出");
            Shell_NotifyIcon(NIM_ADD, &nid);
            break;
        }
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
            wallpaperWindow->paint(hWnd, hdc);
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
        default:
            return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
}
