#pragma once

#include <pre.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
}


class WallpaperWindow {

private:

    HWND hWnd;

    AVFormatContext *fmt_ctx = nullptr;
    const AVCodec *codec = nullptr;
    AVCodecContext *codeCtx = nullptr;
    AVStream *stream = nullptr;
    AVPacket avpkt = {};
    AVFrame *frame = nullptr;
    int stream_index = 0;
    int frame_count = 0;

    void closeVideo();

public:
    double frameTime = 0;
    double nowTime = 0;

    explicit WallpaperWindow(HINSTANCE hInstance);

    ~WallpaperWindow();

    void Show();

    void SetToDesktop();

    void SetVideo(std::string file);

    void ReadFrame();

    void paint(HWND hWnd, HDC hdc);
};
