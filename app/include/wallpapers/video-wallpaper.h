#pragma once

#include "drawers/video-drawer.h"
#include "wallpaper.h"
#include "video-decoder.h"

class VideoWallpaper : public Wallpaper {
private:
    DEVMODE dm;

    std::atomic<VideoDecoder *> decoderPtr;
    VideoDrawer drawer;

    double nowTime;
    double frameTime;
    double lastTime;
protected:


public:
    VideoWallpaper();

    ~VideoWallpaper();

    void draw(SkCanvas *canvas) override;

    void Pause() override;

    void Resume() override;

    bool Paused() const override;

    void Restart() override;

    void SetVideo(const str &file, double seekTime = 0);

    void SeekTo(double time);

    void SetSize(int width, int height) override;
};