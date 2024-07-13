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

/**
 * @brief 视频帧
 *
 * @author YJL
 */
struct VideoFrame {
    /**
     * @brief 帧的位图
     */
    HBITMAP bitmap = nullptr;
    /**
     * @brief 帧的宽度
     */
    int width = 0;
    /**
     * @brief 帧的高度
     */
    int height = 0;
    /**
     * @brief 帧的 presentation timestamp，保留
     */
    int64_t pts = 0;
    /**
     * @brief 帧的时长
     */
    int64_t duration = 0;
};

/**
 * @brief 视频解码器
 *
 * @author YJL
 */
class VideoDecoder {
private:
    AVFormatContext *fmt_ctx = nullptr;
    AVCodec *codec = nullptr;
    AVCodecContext *codeCtx = nullptr;
    AVStream *stream = nullptr;
    AVPacket avpkt = {};
    AVFrame *frame = nullptr;
    int stream_index = -1;
    bool loadedFirstFrame = false;

    std::mutex mtx;
    std::condition_variable cv;

    std::queue<VideoFrame> frames;

    std::atomic<bool> _paused = false;

    /**
     * @brief 解码一帧
     */
    void _decode();

    /**
     * @brief 添加帧
     * @return 是否成功添加
     */
    bool addFrame();

    /**
     * @brief 解码线程
     *
     * @author YJL
     */
    class DecoderThread;

    /**
     * @brief 解码线程
     */
    std::atomic<DecoderThread *> threadPtr = nullptr;

public:

    /**
     * @brief 帧的宽高
     */
    int width = -1, height = -1;

    /**
     * @brief 帧的时间基
     */
    AVRational time_base = {0};

    explicit VideoDecoder(const std::string &file);

    ~VideoDecoder();

    /**
     * @brief 开始解码
     */
    void startDecode();

    /**
     * @brief 关闭解码器
     */
    void close();

    /**
     * @brief 获取一帧
     * @return 一帧或者nullptr（没有可用帧）
     */
    VideoFrame *getFrame();

    int getFrameCount();

    /**
     * @brief 解码器是否正在运行
     * @return 是否正在运行
     */
    bool running() const;

    /**
     * @brief 解码器是否已经解码第一帧
     * @return 是否已经解码第一帧
     */
    bool firstFrameLoaded() const;

    /**
     * @brief 暂停解码
     */
    void pause();

    /**
     * @brief 恢复解码
     */
    void resume();

    /**
     * @brief 解码器是否处于暂停状态
     * @return 解码器是否处于暂停状态
     */
    bool paused() const;

    /**
     * @brief 跳转到指定帧
     *
     * @param time 时间
     */
    void seekTo(double time);

    void waitDecodeNextFrame();
};
