#include <video-decoder.h>

#include <thread>
#include <ios>
#include <sstream>
#include <iomanip>


class VideoDecoder::DecoderThread : public std::thread {

private:
    std::atomic<bool> run{true};

    VideoDecoder *decoder;

    std::mutex mutex;
    std::condition_variable cv;

public:

    explicit DecoderThread(VideoDecoder *decoder) : std::thread([this]() {
        decodeRun();
    }) {
        this->decoder = decoder;
    }

    ~DecoderThread() = default;

    void decodeRun() {
        while (run.load()) {
            try {
                std::unique_lock<std::mutex> lock(mutex);
                cv.wait(lock, [this]() {
                    return !decoder->paused();
                });
                decoder->_decode();
            } catch (...) {
                break;
            }
        }
    }

    void stop() {
        run.store(false);
        decoder->cv.notify_all();
        join();
    }

    void notify() {
        cv.notify_all();
    }

    [[nodiscard]] bool running() const {
        return run.load();
    }
};

VideoDecoder::VideoDecoder(u8str file) {

    int err = avformat_open_input(&fmt_ctx, reinterpret_cast<const char *>(file.c_str()), nullptr, nullptr);
    if (err) {
        char buf[1024];
        av_make_error_string(buf, 1024, err);
        error(std::format(L"Could not open file: {}\nerror: {}", u8str2str(file), string2str(buf)));
    }
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        error(TEXT("Could not find stream info"));
    }
    // av_dump_format(fmt_ctx, 0, file.c_str(), 0);
    av_new_packet(&avpkt, 1024 * 4);
    if (av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0) < 0) {
        error(TEXT("Could not find video stream"));
    }

    // 寻找视频流
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        AVStream *st = fmt_ctx->streams[i];
        if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            stream_index = i;
            break;
        }
    }
    if (stream_index < 0) {
        error(TEXT("No video stream found"));
    }

    stream = fmt_ctx->streams[stream_index];
    codec = (AVCodec *) avcodec_find_decoder(stream->codecpar->codec_id);
    if (!codec) {
        error(TEXT("Could not find video codec"));
    }
    codeCtx = avcodec_alloc_context3(codec);
    if (!codeCtx) {
        error(TEXT("Could not allocate video codec context"));
    }

    if (avcodec_parameters_to_context(codeCtx, stream->codecpar) < 0) {
        error(TEXT("Could not copy codec parameters to codec context"));
    }

    avcodec_open2(codeCtx, codec, nullptr);
    frame = av_frame_alloc();
    if (!frame) {
        error(TEXT("Could not allocate video frame"));
    }

    time_base = stream->time_base;
}

VideoDecoder::~VideoDecoder() {
    av_packet_unref(&avpkt);
    av_frame_free(&frame);
    avcodec_free_context(&codeCtx);
    avformat_close_input(&fmt_ctx);
    avformat_free_context(fmt_ctx);
    while (!frames.empty()) {
        auto vf = frames.front();
        frames.pop();
        free(vf.data);
    }
}


void VideoDecoder::_decode() {

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
                error(std::format(TEXT("Could not decode frame code: {}"), err));
            }
            if (!addFrame())
                return;
        }
        av_packet_unref(&avpkt);
    } else if (r == AVERROR_EOF) {
        frame->best_effort_timestamp = 0;
        av_seek_frame(fmt_ctx, stream_index, 0, AVSEEK_FLAG_BACKWARD);
    }
}

void VideoDecoder::startDecode() {
    threadPtr.store(new DecoderThread(this));
}

void VideoDecoder::close() {
    auto thread = threadPtr.load();
    if (thread) {
        thread->stop();
        delete thread;
        threadPtr.store(nullptr);
    }
}

bool VideoDecoder::addFrame() {
    auto width = Min(frame->width, maxWidth);
    auto height = Min(frame->height, maxHeight);
    struct SwsContext *img_convert_ctx = sws_getContext(
            frame->width, frame->height, static_cast<AVPixelFormat>(frame->format),
            width, height, AV_PIX_FMT_RGBA,
            SWS_BILINEAR, nullptr, nullptr, nullptr);

    if (!img_convert_ctx)
        error(TEXT("Could not initialize the conversion context"));

    AVFrame *rgbFrame = av_frame_alloc();
    rgbFrame->width = width;
    rgbFrame->height = height;
    rgbFrame->format = AV_PIX_FMT_RGBA;
    rgbFrame->duration = frame->duration;
    rgbFrame->pts = frame->pts;
    av_frame_get_buffer(rgbFrame, 32);
    av_frame_copy_props(rgbFrame, frame);

    // 进行颜色空间转换
    sws_scale(img_convert_ctx,
              (const uint8_t *const *) frame->data, frame->linesize, 0, frame->height,
              rgbFrame->data, rgbFrame->linesize);

    int size = rgbFrame->width * rgbFrame->height * 4;
    auto data = (RGBAColor *) malloc(size);
    memcpy(data, rgbFrame->data[0], size);

    struct VideoFrame vf = {
            .data = data,
            .width = rgbFrame->width,
            .height = rgbFrame->height,
            .pts = rgbFrame->pts,
            .duration = rgbFrame->duration,
    };


    av_frame_free(&rgbFrame);
    sws_freeContext(img_convert_ctx);

    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this] { return !running() || frames.size() < MAX_FRAMES; });
    auto thread = threadPtr.load();
    if (thread && thread->running()) {
        frames.push(vf);
    } else {
        lock.unlock();
        return false;
    }
    lock.unlock();
    return true;
}

VideoFrame *VideoDecoder::getFrame() {
    if (frames.empty()) {
        return nullptr;
    }
    std::unique_lock<std::mutex> lock(mtx);
    VideoFrame *vf = &frames.front();
    frames.pop();
    lock.unlock();
    cv.notify_all();
    return vf;
}

bool VideoDecoder::running() const {
    auto thread = threadPtr.load();
    return thread && thread->joinable() && thread->running();
}

bool VideoDecoder::paused() const {
    return _paused.load();
}

void VideoDecoder::pause() {
    _paused.store(true);
}

void VideoDecoder::resume() {
    _paused.store(false);
    auto thread = threadPtr.load();
    if (thread) {
        thread->notify();
    }
}

void VideoDecoder::seekTo(double time) {
    if (time < 0) {
        time = 0;
    }
    auto timestamp = static_cast<int64_t>(time / av_q2d(time_base));
    if (av_seek_frame(fmt_ctx, stream_index, timestamp, AVSEEK_FLAG_BACKWARD) < 0) {
        error_not_throw(std::format(TEXT("Could not seek to {}s"), time));
        return;
    }
    std::unique_lock<std::mutex> lock(mtx);
    while (!frames.empty())
        frames.pop();
    lock.unlock();
    waitDecodeNextFrame();
}

void VideoDecoder::waitDecodeNextFrame() {
    try {
        _decode();
    } catch (...) {
        error_not_throw(TEXT("Could not decode first frame"));
    }
}
