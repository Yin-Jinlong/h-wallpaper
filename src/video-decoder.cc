#include <video-decoder.h>

#include <thread>

VideoDecoder::VideoDecoder(const std::string &file) {
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

    time_base = stream->time_base;
}

VideoDecoder::~VideoDecoder() {
    av_packet_unref(&avpkt);
    av_frame_free(&frame);
    avcodec_free_context(&codeCtx);
    avformat_close_input(&fmt_ctx);
    avformat_free_context(fmt_ctx);
}


HBITMAP avframe_to_hbitmap(AVFrame *frame) {
    HDC hdc = GetDC(nullptr);

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
    ReleaseDC(nullptr, hdc);
    return hBitmap;
}

void VideoDecoder::_decode() {
    while (run) {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck, [this] { return frmaes.size() < 60; });

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
                addFrame();
            }
            av_packet_unref(&avpkt);
        } else if (r == AVERROR_EOF) {
            av_seek_frame(fmt_ctx, 0, 0, AVSEEK_FLAG_BACKWARD);
        }
    }
}

void VideoDecoder::startDecode() {
    std::thread([this]() {
        _decode();
    }).detach();
}

void VideoDecoder::close() {
    run = false;
}

void VideoDecoder::addFrame() {
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

    struct VideoFrame vf = {
            .bitmap = avframe_to_hbitmap(rgbFrame),
            .width = frame->width,
            .height = frame->height,
            .pts = frame->pts,
            .duration = frame->duration
    };
    frmaes.push(vf);

    av_frame_free(&rgbFrame);
    sws_freeContext(img_convert_ctx);
}

VideoFrame *VideoDecoder::getFrame() {
    std::lock_guard<std::mutex> lck(mtx);
    if (frmaes.empty())
        return nullptr;
    VideoFrame *vf = &frmaes.front();
    frmaes.pop();
    cv.notify_one();
    return vf;
}

int VideoDecoder::getFrameCount() {
    return frmaes.size();
}
