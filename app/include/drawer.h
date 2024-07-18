#pragma once

#include <pre.h>

#include <include/core/SkBitmap.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkColor.h>
#include <include/core/SkPaint.h>
#include <include/core/SkSurface.h>

#include "video-decoder.h"
#include "config.h"

class Drawer {
private:
    sk_sp<SkSurface> surface;

    SkCanvas *canvas = nullptr;

    SkPaint paint;

    BITMAPINFO bmi{0};

    float width = 0, height = 0;

    double rate = 0;

public:

    Drawer();

    /**
     * @brief 绘制帧
     * @param hdc 内存hdc
     * @param frame 帧
     * @return 是否绘制了
     */
    bool Draw(HDC hdc, VideoFrame *frame);

    void DrawImage(SkImage *image, ContentFit fit);

    void SetSize(int width, int height);
};
