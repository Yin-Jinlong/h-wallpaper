#include <wallpaper-window.h>
#include <ctime>

#include "resources.h"
#include "wnd-utils.h"
#include "string-table.h"
#include "sys-err.h"
#include "wallpapers/video-wallpaper.h"
#include "image-util.h"

#define FIT_MENU_ID_START 100

#define PMID_EXIT 1
#define PMID_CHANGE 2
#define PMID_RUN_ON_STARTUP 3
#define PMID_CHECK_UPDATE 4
#define PMID_CHECK_UPDATE_ON_START 5
#define PMID_FIT_MENU(fit) (((int)ContentFit::fit)+FIT_MENU_ID_START)

#define FIT_MENU_ITEM(fit) checkedFlag(ContentFit::fit), PMID_FIT_MENU(fit)
#define CHECK_FIT_MENU_ITEM(fit) PMID_FIT_MENU(fit), checkedFlag(ContentFit::fit)

#define REG_RUN_KEY TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run")

static const WCHAR *HWallpaperWindowClassName = TEXT("YJL-WALLPAPER");

extern std::string appPath;
extern str exeWPath;

WallpaperWindow *wallpaperWindow;

namespace hww {
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
    void startPaint(HDC hdc) {
        // 没有壁纸，绘制黑色
        auto wpp = wallpaperWindow->GetWallpaper();
        if (!wpp) {
            HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
            RECT rect = {0};
            rect.right = wallpaperWindow->GetWidth();
            rect.bottom = wallpaperWindow->GetHeight();
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);
            return;
        }

        HDC mdc = CreateCompatibleDC(hdc);
        wallpaperWindow->paint(mdc);

        BitBlt(hdc, 0, 0,
               wallpaperWindow->GetWidth(),
               wallpaperWindow->GetHeight(),
               mdc, 0, 0, SRCCOPY);

        DeleteDC(mdc);
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
        AppendMenu(trayFitMenu, FIT_MENU_ITEM(PIP), GET_CSTR(IDS_FIT_MENU_PIP));


        AppendMenu(trayMenu, MF_UNCHECKED, PMID_RUN_ON_STARTUP, GET_CSTR(IDS_RUN_ON_STARTUP));
        AppendMenu(trayMenu, MF_SEPARATOR, 0, nullptr);

        AppendMenu(trayMenu, MF_STRING, PMID_CHECK_UPDATE, GET_CSTR(IDS_CHECK_UPDATE));
        AppendMenu(trayMenu, MF_UNCHECKED, PMID_CHECK_UPDATE_ON_START, GET_CSTR(IDS_CHECK_UPDATE_ON_START));
        AppendMenu(trayMenu, MF_SEPARATOR, 0, nullptr);

        AppendMenu(trayMenu, MF_POPUP, (UINT_PTR) trayFitMenu, GET_CSTR(IDS_FIT_MENU));
        AppendMenu(trayMenu, MF_STRING, PMID_CHANGE, GET_CSTR(IDS_CHOOSE_FILE));
        AppendMenu(trayMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenu(trayMenu, MF_STRING, PMID_EXIT, GET_CSTR(IDS_EXIT));

        Shell_NotifyIcon(NIM_ADD, &nid);
    }

    void updateCheckMenuStatus(HMENU menu, int id, bool checked) {
        CheckMenuItem(menu, id, checked ? MF_CHECKED : MF_UNCHECKED);
    }

    void updateCheckMenu() {
        updateCheckMenuStatus(trayMenu, PMID_RUN_ON_STARTUP, isRunOnStartup());
        updateCheckMenuStatus(trayMenu, PMID_CHECK_UPDATE_ON_START, config.update.checkOnStart);

        CheckMenuItem(trayFitMenu, CHECK_FIT_MENU_ITEM(CLIP));
        CheckMenuItem(trayFitMenu, CHECK_FIT_MENU_ITEM(CONTAIN));
        CheckMenuItem(trayFitMenu, CHECK_FIT_MENU_ITEM(STRETCH));
        CheckMenuItem(trayFitMenu, CHECK_FIT_MENU_ITEM(CENTER));
        CheckMenuItem(trayFitMenu, CHECK_FIT_MENU_ITEM(REPEAT));
        CheckMenuItem(trayFitMenu, CHECK_FIT_MENU_ITEM(PIP));
    }

    void onMenuClick(HWND hWnd, int id) {
        if (id >= FIT_MENU_ID_START) {
            config.wallpaper.fit = static_cast<ContentFit>(id - FIT_MENU_ID_START);
            SaveConfig();
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
                        auto wpp = (VideoWallpaper *) wallpaperWindow->GetWallpaper();
                        wpp->SetVideo(ofn.lpstrFile);
                        wallpaperWindow->Redraw();
                    }
                    break;
                }
                case PMID_RUN_ON_STARTUP: {
                    setRunOnStartup(!isRunOnStartup());
                    break;
                }
                case PMID_CHECK_UPDATE: {
                    STARTUPINFO si;
                    ZeroMemory(&si, sizeof(si));
                    si.cb = sizeof(si);
                    si.dwFlags = STARTF_USESHOWWINDOW;
                    si.wShowWindow = SW_HIDE;

                    PROCESS_INFORMATION pi;
                    auto cmd = std::format(TEXT("{}-updater"), APP_NAME);
                    if (!CreateProcess(nullptr,
                                       cmd.data(),
                                       nullptr, nullptr,
                                       TRUE, 0,
                                       nullptr, nullptr,
                                       &si, &pi)) {
                        error_message(GetLastError());
                        break;
                    }
                    CloseHandle(pi.hThread);
                    break;
                }
                case PMID_CHECK_UPDATE_ON_START:
                    config.update.checkOnStart = !config.update.checkOnStart;
                    SaveConfig();
                    break;
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

    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = 0;
    bmi.bmiHeader.biHeight = 0;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    wallpaperPtr.store(new VideoWallpaper());
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

WallpaperWindow::~WallpaperWindow() {
    SaveConfig();
}

void WallpaperWindow::paint(HDC hdc) {
    auto wallpaper = wallpaperPtr.load();
    if (!wallpaper)
        return;

    auto canvas = surface->getCanvas();
    wallpaper->draw(canvas);

    drawToHdc(hdc);
}

void WallpaperWindow::SetSize(int width, int height) {
    this->width = width;
    this->height = height;
    auto wallpaper = wallpaperPtr.load();
    if (wallpaper)
        wallpaper->SetSize(width, height);

    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    SkImageInfo info = SkImageInfo::Make(
            width, height,
            kRGBA_8888_SkColorType, kOpaque_SkAlphaType
    );
    surface = SkSurfaces::Raster(info);
}

void WallpaperWindow::pause() {
    auto wallpaper = wallpaperPtr.load();
    if (wallpaper) {
        wallpaper->Pause();
    }
}

void WallpaperWindow::resume() {
    auto wallpaper = wallpaperPtr.load();
    if (wallpaper) {
        wallpaper->Resume();
    }
    Redraw();
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

void WallpaperWindow::Redraw() {
    InvalidateRect(hWnd, nullptr, false);
}

Wallpaper *WallpaperWindow::GetWallpaper() {
    return wallpaperPtr.load();
}

bool WallpaperWindow::Paused() const {
    auto wpp = wallpaperPtr.load();
    if (wpp)
        return wpp->Paused();
    return false;
}

void WallpaperWindow::drawToHdc(HDC hdc) {
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, nullptr, nullptr, 0);
    SelectObject(hdc, hBitmap);

    auto pixels = SkImageGetBgr888Pixels(surface->makeImageSnapshot().get());

    if (pixels) {
        SetDIBits(hdc, hBitmap, 0, height, pixels, &bmi, DIB_RGB_COLORS);
        free(pixels);
    }
    DeleteObject(hBitmap);
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
            startPaint(hdc);
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
                updateCheckMenu();
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
        case WM_APP_QUERY_MAXIMIZED: {
            static time_t lastSaveTime = 0;
            // 每5s播放保存一次配置
            // 以免在意外结束进程后丢失播放进度
            if (!wallpaperWindow->Paused()) {
                auto nowTime = time(nullptr);
                if (nowTime - lastSaveTime > 5) {
                    SaveConfig();
                    lastSaveTime = nowTime;
                }
            }
            if (HasWindowMaximized()) {
                if (!wallpaperWindow->Paused())
                    wallpaperWindow->pause();
            } else if (wallpaperWindow->Paused()) {
                wallpaperWindow->resume();
            }
            break;
        }
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
                auto wpp = (VideoWallpaper *) wallpaperWindow->GetWallpaper();
                wpp->SetVideo(u8str2str(file));
            }
            break;
        }
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

#undef REG_RUN_KEY
