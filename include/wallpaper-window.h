#pragma once

#include <pre.h>

#include "video-decoder.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
}


class WallpaperWindow {

private:

    HWND hWnd;

    VideoDecoder *decoder = nullptr;

public:
    double frameTime = 0;
    double nowTime = 0;

    explicit WallpaperWindow(HINSTANCE hInstance);

    ~WallpaperWindow();

    void Show();

    void SetToDesktop();

    void SetVideo(std::string file);

    void paint(HWND hWnd, HDC hdc);
};
