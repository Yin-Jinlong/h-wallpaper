#include <wallpapers/video-wallpaper.h>
#include "wallpaper-window.h"

extern WallpaperWindow *wallpaperWindow;


VideoWallpaper::VideoWallpaper() :
        Wallpaper(WallpaperType::WALLPAPER_TYPE_VIDEO) {
    dm.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dm);
}

VideoWallpaper::~VideoWallpaper() {

}

void VideoWallpaper::SetVideo(const str &file, double seekTime) {
    if (file.empty())
        return;
    VideoDecoder *nd;
    try {
        Pause();
        nd = new VideoDecoder(str2u8str(file));
    } catch (...) {
        Resume();
        return;
    }
    {
        auto decoder = decoderPtr.load();
        if (decoder) {
            decoder->close();
            delete decoder;
            decoderPtr.store(nullptr);
        }
    }
    nowTime = 0;
    frameTime = 0;
    lastTime = 0;
    nd->maxWidth = width;
    nd->maxHeight = height;
    decoderPtr.store(nd);
    nd->waitDecodeNextFrame();
    requestRedraw();
    if (seekTime > 0) {
        SeekTo(seekTime);
    }
    nd->startDecode();
}

void VideoWallpaper::Pause() {
    auto decoder = decoderPtr.load();
    if (decoder) {
        decoder->pause();
    }
}

void VideoWallpaper::Resume() {
    auto decoder = decoderPtr.load();
    if (decoder) {
        lastTime = 0;
        decoder->resume();
    }
}

void VideoWallpaper::Restart() {

}

double toTime(SYSTEMTIME t) {
    return (t.wHour * 3600 + t.wMinute * 60 + t.wSecond) + t.wMilliseconds / 1000.0;
}


void VideoWallpaper::draw(HDC mdc) {

    SYSTEMTIME now;
    GetSystemTime(&now);
    double dnt = toTime(now) - lastTime;
    if (lastTime == 0) {
        dnt = 1.0 / dm.dmDisplayFrequency;
    }
    lastTime = toTime(now);
    // 时间差过大，则按1s算
    nowTime += dnt > 1 ? 1 : dnt;

    auto decoder = decoderPtr.load();
    if (!decoder || frameTime > nowTime) {
        return;
    }

    auto vf = decoder->getFrame();
    if (!vf || !vf->data) {
        requestRedraw();
        return;
    }

    config.wallpaper.time = av_q2d(decoder->time_base) * vf->pts;
    auto dt = av_q2d(decoder->time_base) * vf->duration;
    frameTime += dt;

    if (drawer.Draw(mdc, vf))
        requestRedraw();
}

void VideoWallpaper::SeekTo(double time) {
    auto decoder = decoderPtr.load();
    if (decoder && time >= 0) {
        decoder->seekTo(time);
        nowTime = time;
        frameTime = time;
    }
}

bool VideoWallpaper::Paused() const {
    auto decoder = decoderPtr.load();
    if (decoder) {
        return decoder->paused();
    }
    return Wallpaper::Paused();
}

void VideoWallpaper::SetSize(int width, int height) {
    Wallpaper::SetSize(width, height);
    drawer.SetSize(width, height);
}
