#pragma once

#include <pre.h>

#include <include/effects/SkImageFilters.h>

#include <map>

#include "config.h"
#include "drawer.h"
#include "video-decoder.h"

typedef void (*DrawImageFn)(SkCanvas *, SkImage *, SkPaint *);

class VideoDrawer : public Drawer {
private:
    sk_sp<SkImageFilter> pipFilter;
    SkPaint pipPaint;

    VideoFrame *frame = nullptr;

    const std::map<ContentFit, DrawImageFn> drawFnMap = {
        {ContentFit::CLIP, drawImageClip},
        {ContentFit::CONTAIN, drawImageContain},
        {ContentFit::STRETCH, drawImageStretch},
        {ContentFit::CENTER, drawImageCenter},
        {ContentFit::REPEAT, drawImageRepeat},
        {ContentFit::PIP, drawImagePip},
    };

private:
    static void drawImageClip(SkCanvas *canvas, SkImage *image, SkPaint *paint = nullptr);
    static void drawImageContain(SkCanvas *canvas, SkImage *image, SkPaint *paint = nullptr);
    static void drawImageStretch(SkCanvas *canvas, SkImage *image, SkPaint *paint = nullptr);
    static void drawImageCenter(SkCanvas *canvas, SkImage *image, SkPaint *paint = nullptr);
    static void drawImageRepeat(SkCanvas *canvas, SkImage *image, SkPaint *paint = nullptr);
    static void drawImagePip(SkCanvas *canvas, SkImage *image, SkPaint *paint = nullptr);

public:
    VideoDrawer();

    /**
     * @brief 设置当前帧
     * @param frame 帧
     */
    void SetFrame(VideoFrame *frame);

    /**
     * @brief 绘制帧
     * @param canvas 画布，不能为空
     * @param frame 帧
     * @return 是否绘制了
     */
    bool Draw(SkCanvas *canvas) override;

    /**
     * @brief 绘制图片
     * @param canvas 画布，不能为空
     * @param image 图片
     * @param fit 适应
     * @param paint 画笔
     * @see ContentFit
     */
    void DrawImage(SkCanvas *canvas, SkImage *image, ContentFit fit, SkPaint *paint = nullptr);
};
