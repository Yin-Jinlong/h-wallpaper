#pragma once

#include <pre.h>

#include <include/core/SkBitmap.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkColor.h>
#include <include/core/SkPaint.h>
#include <include/core/SkSurface.h>
#include <include/effects/SkImageFilters.h>

#include "video-decoder.h"
#include "config.h"

class Drawer {
private:

    sk_sp<SkImageFilter> pipFilter;
    SkPaint pipPaint;

    float width = 0, height = 0;

    double rate = 0;

public:

    Drawer();

    /**
     * @brief 绘制帧
     * @param canvas 画布，不能为空
     * @param frame 帧
     * @return 是否绘制了
     */
    bool Draw(SkCanvas *canvas, VideoFrame *frame);

    void DrawImage(SkCanvas *canvas, SkImage *image, ContentFit fit, SkPaint *paint = nullptr);

    void SetSize(int width, int height);
};
