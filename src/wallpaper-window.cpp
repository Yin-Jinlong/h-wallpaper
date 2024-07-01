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
    if (frame) {
        closeVideo();
        frameTime = 0;
        nowTime = 0;
        frame_count = 0;
    }

    if (avformat_open_input(&fmt_ctx, file.c_str(), nullptr, nullptr)) {
        error(L"Could not open file");
    }
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        error(L"Could not find stream info");
    }
    // av_dump_format(fmt_ctx, 0, file.c_str(), 0);
    av_new_packet(&avpkt, 1024 * 4);
    if (av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0) < 0) {
        error(L"Could not find video stream");
    }
    stream = fmt_ctx->streams[stream_index];
    codec = (AVCodec *) avcodec_find_decoder(stream->codecpar->codec_id);
    if (!codec) {
        error(L"Could not find video codec");
    }
    codeCtx = avcodec_alloc_context3(codec);
    if (!codeCtx) {
        error(L"Could not allocate video codec context");
    }

    if (avcodec_parameters_to_context(codeCtx, stream->codecpar) < 0) {
        error(L"Could not copy codec parameters to codec context");
    }

    avcodec_open2(codeCtx, codec, nullptr);
    frame = av_frame_alloc();
    if (!frame) {
        error(L"Could not allocate video frame");
    }
}

void WallpaperWindow::closeVideo() {
    avformat_close_input(&fmt_ctx);
    avformat_free_context(fmt_ctx);
    av_frame_free(&frame);
    av_packet_unref(&avpkt);
    avcodec_close(codeCtx);
    avcodec_free_context(&codeCtx);
}

WallpaperWindow::~WallpaperWindow() {
    closeVideo();
}

void WallpaperWindow::ReadFrame() {
    if (!frame || frameTime > nowTime)
        return;
    auto t = av_q2d(stream->time_base) * frame->duration;
    frameTime += t;
    start:
    int r = av_read_frame(fmt_ctx, &avpkt);
    if (r >= 0) {
        if (avpkt.stream_index == stream_index) {
            int err;
            load:
            err = avcodec_send_packet(codeCtx, &avpkt);
            if (err == -1094995529)
                goto start;

            err = avcodec_receive_frame(codeCtx, frame);
            if (err == AVERROR(EAGAIN))
                goto load;
            else if (err != 0) {
                WCHAR err_str[256];
                swprintf_s(err_str, 256, L"%d", err);
                error(err_str);
            }
            frame_count++;
        }
        av_packet_unref(&avpkt);
    } else if (r == AVERROR_EOF) {
        av_seek_frame(fmt_ctx, 0, 0, AVSEEK_FLAG_BACKWARD);
        nowTime = 0;
        frameTime = 0;
    }
}

HBITMAP avframe_to_hbitmap(HDC hdc, AVFrame *frame) {
    BITMAPINFOHEADER bmiHeader;
    memset(&bmiHeader, 0, sizeof(BITMAPINFOHEADER));
    bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiHeader.biWidth = frame->width;
    bmiHeader.biHeight = -frame->height; // 注意这里是负数，因为AVFrame的原点在左上角，而GDI的原点在左下角
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 24;
    bmiHeader.biCompression = BI_RGB;

    HBITMAP hBitmap = CreateDIBSection(hdc, (BITMAPINFO *) &bmiHeader, DIB_RGB_COLORS, NULL, NULL, 0);

    void *bits = malloc(frame->height * frame->width * 3);
    GetDIBits(hdc, hBitmap, 0, frame->height, bits, (BITMAPINFO *) &bmiHeader, DIB_RGB_COLORS);

    memcpy(bits, frame->data[0], frame->height * frame->width * 3);

    SetDIBits(hdc, hBitmap, 0, frame->height, bits, (BITMAPINFO *) &bmiHeader, DIB_RGB_COLORS);
    SelectObject(hdc, hBitmap);
    free(bits);
    return hBitmap;
}

void WallpaperWindow::paint(HWND hWnd, HDC hdc) {
    if (!frame)
        return;

    HDC mdc = CreateCompatibleDC(hdc);

    struct SwsContext *img_convert_ctx = sws_getContext(
            frame->width, frame->height, static_cast<AVPixelFormat>(frame->format),
            frame->width, frame->height, AV_PIX_FMT_BGR24,
            SWS_BILINEAR, nullptr, nullptr, nullptr);

    AVFrame *rgbFrame = av_frame_alloc();
    rgbFrame->width = frame->width;
    rgbFrame->height = frame->height;
    rgbFrame->format = AV_PIX_FMT_BGR24;
    av_frame_get_buffer(rgbFrame, 32);

    // 进行颜色空间转换
    sws_scale(img_convert_ctx,
              (const uint8_t *const *) frame->data, frame->linesize, 0, frame->height,
              rgbFrame->data, rgbFrame->linesize);

    HBITMAP hBitmap = avframe_to_hbitmap(mdc, rgbFrame);

    SelectObject(mdc, hBitmap);

    RECT rect;
    GetWindowRect(hWnd, &rect);

    StretchBlt(hdc, 0, 0, rect.right, rect.bottom, mdc, 0, 0, frame->width, frame->height, SRCCOPY);

    DeleteObject(hBitmap);
    av_frame_free(&rgbFrame);
    sws_freeContext(img_convert_ctx);

    DeleteDC(mdc);
}

DEVMODE dm;
HMENU trayMenu;
NOTIFYICONDATA nid;

double lastTime = 0;

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

            SYSTEMTIME now;
            GetSystemTime(&now);
            double dt = toTime(now) - lastTime;
            if (lastTime == 0) {
                dt = 1.0 / dm.dmDisplayFrequency;
            }
            lastTime = toTime(now);

            wallpaperWindow->nowTime += dt;
            wallpaperWindow->ReadFrame();
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            wallpaperWindow->paint(hWnd, hdc);
            EndPaint(hWnd, &ps);
            InvalidateRect(hWnd, nullptr, FALSE);
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
