#include <wallpaper-window.h>

#include "resources.h"
#include "wnd-utils.h"
#include "string-table.h"
#include "sys-err.h"
#include "image-util.h"

#define FIT_MENU_ID_START 100

#define PMID_EXIT 1
#define PMID_CHANGE 2
#define PMID_RUN_ON_STARTUP 3
#define PMID_FIT_MENU(fit) (((int)ContentFit::fit)+FIT_MENU_ID_START)

#define FIT_MENU_ITEM(fit) checkedFlag(ContentFit::fit), PMID_FIT_MENU(fit)
#define CHECK_FIT_MENU_ITEM(fit) PMID_FIT_MENU(fit), checkedFlag(ContentFit::fit)

#define REG_RUN_KEY TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run")

static const WCHAR *HWallpaperWindowClassName = TEXT("YJL-WALLPAPER");

extern std::string appPath;
extern str exeWPath;

WallpaperWindow *wallpaperWindow;

namespace hww {
    DEVMODE dm;
    HMENU trayMenu;
    HMENU trayFitMenu;
    NOTIFYICONDATA nid;
    HANDLE hMapFile = nullptr;

    LRESULT windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void registerWndClass(HINSTANCE hInstance) {
        WNDCLASSEX wndClass;
        wndClass.cbSize = sizeof(wndClass);
        wndClass.style = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpfnWndProc = windowProc;
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
            error(TEXT("RegisterClassEx failed"));
        }
    }

    HWND pmWindow() {
        return FindWindow(TEXT("Progman"), TEXT("Program Manager"));
    }

    HWND splitDesktopWindow() {
        HWND hWnd = pmWindow();
        if (hWnd != nullptr) {
            SendMessage(hWnd, 0x052C, 0, 0);
        }
        return hWnd;
    }

    double toTime(SYSTEMTIME t) {
        return (t.wHour * 3600 + t.wMinute * 60 + t.wSecond) + t.wMilliseconds / 1000.0;
    }

    void createMapping() {
        hMapFile = CreateFileMapping(
                INVALID_HANDLE_VALUE,
                nullptr,
                PAGE_READWRITE,
                0,
                sizeof(double),
                HW_FM_VIDEO);
        if (hMapFile == nullptr) {
            error(TEXT("CreateFileMappingW failed"));
        }
    }

    /**
     * 绘制
     * @param hdc
     * @return 是否重绘
     */
    bool startPaint(HDC hdc) {
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

        HDC mdc = CreateCompatibleDC(hdc);
        if (wallpaperWindow->paint(mdc)) {
            BitBlt(hdc, 0, 0,
                   wallpaperWindow->GetWidth(),
                   wallpaperWindow->GetHeight(),
                   mdc, 0, 0, SRCCOPY);
        }
        DeleteDC(mdc);
        return true;
    }

    bool regHasValue(HKEY hkey, LPCWSTR subKey, LPCWSTR keyName) {
        auto r = RegGetValue(hkey, subKey, keyName, RRF_RT_ANY, nullptr, nullptr, nullptr);
        error_message(r);
        return r == ERROR_SUCCESS;
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
            error_message(err);
            error(TEXT("RegQueryValueEx failed"));
        }
        return exeWPath == value;
    }

    void setRunOnStartup(bool run) {
        HKEY runKey;
        DWORD err;
        if ((err = RegOpenKeyEx(HKEY_CURRENT_USER, REG_RUN_KEY, 0, KEY_ALL_ACCESS, &runKey))) {
            error(TEXT("RegOpenKeyEx failed"));
            error_message(err);
        }
        if (run) {
            if ((err = RegSetValueEx(runKey, APP_NAME, 0, REG_SZ,
                                     (BYTE *) exeWPath.c_str(),
                                     static_cast<DWORD>(exeWPath.size() * sizeof(WCHAR))))) {
                RegCloseKey(runKey);
                error(TEXT("RegSetValueEx failed"));
                error_message(err);
            }
        } else {
            if (regHasValue(runKey, nullptr, APP_NAME)) {
                if ((err = RegDeleteValue(runKey, APP_NAME))) {
                    RegCloseKey(runKey);
                    error(TEXT("RegDeleteValue failed"));
                    error_message(err);
                }
            }
        }
        RegCloseKey(runKey);
    }

    int checkedFlag(ContentFit fit) {
        return fit == config.wallpaper.fit ? MF_CHECKED : MF_UNCHECKED;
    }

    void createTray(HWND hWnd) {
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

        trayFitMenu = CreatePopupMenu();

        AppendMenu(trayFitMenu, FIT_MENU_ITEM(CLIP), GET_CSTR(IDS_FIT_MENU_CLIP));
        AppendMenu(trayFitMenu, FIT_MENU_ITEM(CONTAIN), GET_CSTR(IDS_FIT_MENU_CONTAIN));
        AppendMenu(trayFitMenu, FIT_MENU_ITEM(STRETCH), GET_CSTR(IDS_FIT_MENU_STRETCH));
        AppendMenu(trayFitMenu, FIT_MENU_ITEM(CENTER), GET_CSTR(IDS_FIT_MENU_CENTER));
        AppendMenu(trayFitMenu, FIT_MENU_ITEM(REPEAT), GET_CSTR(IDS_FIT_MENU_REPEAT));

        AppendMenu(trayMenu,
                   isRunOnStartup() ? MF_CHECKED : MF_UNCHECKED, PMID_RUN_ON_STARTUP,
                   GET_CSTR(IDS_RUN_ON_STARTUP));
        AppendMenu(trayMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenu(trayMenu, MF_POPUP, (UINT_PTR) trayFitMenu, GET_CSTR(IDS_FIT_MENU));
        AppendMenu(trayMenu, MF_STRING, PMID_CHANGE, GET_CSTR(IDS_CHOOSE_FILE));
        AppendMenu(trayMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenu(trayMenu, MF_STRING, PMID_EXIT, GET_CSTR(IDS_EXIT));
        Shell_NotifyIcon(NIM_ADD, &nid);
    }

    void onMenuClick(HWND hWnd, int id) {
        if (id >= FIT_MENU_ID_START) {
            config.wallpaper.fit = static_cast<ContentFit>(id - FIT_MENU_ID_START);
        } else {
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
                        wallpaperWindow->SetVideo(str2u8str(ofn.lpstrFile));
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
    }
}

using namespace hww;

WallpaperWindow::WallpaperWindow(HINSTANCE hInstance) {
    registerWndClass(hInstance);
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
        error(TEXT("CreateWindowEx failed"));
        error_message(GetLastError());
    }

    avformat_network_init();
}

void WallpaperWindow::Show() {
    ShowWindow(hWnd, SW_MAXIMIZE);
    UpdateWindow(hWnd);
}

HWND WallpaperWindow::FindExist() {
    HWND hwnd = FindWindowEx(pmWindow(), nullptr, HWallpaperWindowClassName, nullptr);
    return hwnd;
}

void WallpaperWindow::SetToDesktop() {
    HWND desktop = splitDesktopWindow();
    EnumWindows([](HWND hwnd, LPARAM _) -> BOOL {
        if (FindWindowEx(hwnd, nullptr, TEXT("SHELLDLL_DefView"), nullptr) != nullptr) {
            ShowWindow(FindWindowEx(nullptr, hwnd, TEXT("WorkerW"), nullptr), SW_HIDE);
            return FALSE;
        }
        return TRUE;
    }, 0);
    SetParent(hWnd, desktop);
}

void WallpaperWindow::SetVideo(const u8str &file, bool save, double seekTime) {
    if (file.empty())
        return;
    VideoDecoder *nd;
    try {
        wallpaperWindow->pause();
        nd = new VideoDecoder(file);
    } catch (...) {
        wallpaperWindow->resume();
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
    nd->maxWidth = width;
    nd->maxHeight = height;
    decoderPtr.store(nd);
    if (save) {
        config.wallpaper.file = file;
        SaveConfig();
    }
    nd->waitDecodeNextFrame();
    if (seekTime > 0) {
        SeekTo(seekTime);
    }
    nd->startDecode();
    InvalidateRect(hWnd, nullptr, false);
}

WallpaperWindow::~WallpaperWindow() {
    SaveConfig();

    auto decoder = decoderPtr.load();
    if (decoder) {
        decoder->close();
        delete decoder;
        decoderPtr.store(nullptr);
    }
}

bool WallpaperWindow::paint(HDC hdc) {
    auto decoder = decoderPtr.load();
    if (!decoder || frameTime > nowTime)
        return false;

    auto vf = decoder->getFrame();
    if (!vf || !vf->data)
        return false;
    config.wallpaper.time = av_q2d(decoder->time_base) * vf->pts;
    auto dt = av_q2d(decoder->time_base) * vf->duration;
    frameTime += dt;

    return drawer.Draw(hdc, vf);
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
    drawer.SetSize(width, height);
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
        wallpaperWindow->lastTime = 0;
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

void WallpaperWindow::SeekTo(double time) {
    auto decoder = decoderPtr.load();
    if (decoder) {
        decoder->seekTo(time);
        nowTime = time;
        frameTime = time;
    }
}

LRESULT hww::windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            createTray(hWnd);

            createMapping();
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
            if (startPaint(hdc)) {
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            EndPaint(hWnd, &ps);
            break;
        }
        case WM_USER: {
            POINT pt;
            GetCursorPos(&pt);
            if (lParam == WM_LBUTTONDOWN) {
                onMenuClick(hWnd, PMID_CHANGE);
            } else if (lParam == WM_RBUTTONDOWN) {
                SetForegroundWindow(hWnd);
                CheckMenuItem(trayMenu, PMID_RUN_ON_STARTUP, isRunOnStartup() ? MF_CHECKED : MF_UNCHECKED);
                CheckMenuItem(trayFitMenu, CHECK_FIT_MENU_ITEM(CLIP));
                CheckMenuItem(trayFitMenu, CHECK_FIT_MENU_ITEM(CONTAIN));
                CheckMenuItem(trayFitMenu, CHECK_FIT_MENU_ITEM(STRETCH));
                CheckMenuItem(trayFitMenu, CHECK_FIT_MENU_ITEM(CENTER));
                CheckMenuItem(trayFitMenu, CHECK_FIT_MENU_ITEM(REPEAT));
                int id = TrackPopupMenuEx(trayMenu, TPM_RETURNCMD, pt.x, pt.y, hWnd, nullptr);
                onMenuClick(hWnd, id);
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
            } else if (wallpaperWindow->decoderPaused()) {
                wallpaperWindow->resume();
            }
            break;
        case WM_APP_VIDEO_FILE: {
            char *pData = (char *) MapViewOfFile(
                    hMapFile,
                    FILE_MAP_READ, 0, 0, 0
            );
            if (pData) {
                auto len = ((uint16_t *) pData)[0];
                auto file = new char8_t[len + 1];
                memcpy(file, pData + 2, len);
                file[len] = '\0';
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
