#pragma once

#include <pre.h>

#include <queue>
#include <mutex>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
}

struct VideoFrame {
    HBITMAP bitmap = nullptr;
    int width = 0;
    int height = 0;
    int64_t pts = 0;
    int64_t duration = 0;
};

class VideoDecoder {
private:
    AVFormatContext *fmt_ctx = nullptr;
    AVCodec *codec = nullptr;
    AVCodecContext *codeCtx = nullptr;
    AVStream *stream = nullptr;
    AVPacket avpkt = {};
    AVFrame *frame = nullptr;
    int stream_index = 0;
    int frame_count = 0;

    bool run = true;

    std::mutex mtx;
    std::condition_variable cv;

    std::queue<VideoFrame> frmaes;

    void _decode();

    void addFrame();

public:
    AVRational time_base;

    explicit VideoDecoder(const std::string &file);

    ~VideoDecoder();

    void startDecode();

    void close();

    VideoFrame* getFrame();

    int getFrameCount();
};
