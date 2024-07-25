#pragma once

#include "drawers/video-drawer.h"
#include "video-decoder.h"
#include "wallpaper.h"

/**
 * @brief 视频壁纸
 * @author YJL
 */
class VideoWallpaper : public Wallpaper {
private:
    /**
     * @brief 屏幕参数
     */
    DEVMODE dm{0};

    /**
     * @brief 视频解码器
     */
    std::atomic<VideoDecoder *> decoderPtr;
    /**
     * @brief 视频绘图器
     */
    VideoDrawer drawer;

    /**
     * @brief 当前应该播放时间
     */
    double nowTime = 0;
    /**
     * @brief 当前帧时间
     */
    double frameTime = 0;
    /**
     * @brief 绘制上一帧的帧时间
     */
    double lastTime = 0;

protected:
public:
    VideoWallpaper();

    ~VideoWallpaper();

    void draw(SkCanvas *canvas) override;

    void Pause() override;

    void Resume() override;

    bool Paused() const override;

    void Restart() override;

    bool SetVideo(const str &file, double seekTime = 0);

    void SeekTo(double time);

    void SetSize(int width, int height) override;
};