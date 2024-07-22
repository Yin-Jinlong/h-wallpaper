#pragma once

#include <pre.h>

#include <include/effects/SkImageFilters.h>

#include "drawer.h"
#include "video-decoder.h"
#include "config.h"

class VideoDrawer : public Drawer {
private:

    sk_sp<SkImageFilter> pipFilter;
    SkPaint pipPaint;

    VideoFrame *frame;

public:

    VideoDrawer();

    void SetFrame(VideoFrame *frame);

    /**
     * @brief 绘制帧
     * @param canvas 画布，不能为空
     * @param frame 帧
     * @return 是否绘制了
     */
    bool Draw(SkCanvas *canvas) override;

    void DrawImage(SkCanvas *canvas, SkImage *image, ContentFit fit, SkPaint *paint = nullptr);

};
