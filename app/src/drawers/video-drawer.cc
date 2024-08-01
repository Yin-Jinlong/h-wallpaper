#include "image-util.h"
#include <drawers/video-drawer.h>


VideoDrawer::VideoDrawer() {
    pipFilter = SkImageFilters::Blur(10, 10, nullptr);
    pipPaint.setImageFilter(pipFilter);
}


bool VideoDrawer::Draw(SkCanvas *canvas) {
    if (!frame)
        return false;

    SkImageInfo info = SkImageInfo::Make(frame->width, frame->height, kRGBA_8888_SkColorType, kOpaque_SkAlphaType);
    SkBitmap frameBitmap;
    if (!frameBitmap.installPixels(info, frame->data, info.minRowBytes())) {
        free(frame->data);
        frame->data = nullptr;
        return false;
    }

    canvas->clear(SK_ColorBLACK);
    auto width = (float) canvas->getSurface()->width();
    auto height = (float) canvas->getSurface()->height();
    DrawImage(canvas, width, height, frameBitmap.asImage().get(), config.wallpaper.fit);

    free(frame->data);
    frame->data = nullptr;
    return true;
}

void VideoDrawer::drawImageClip(SkCanvas *canvas, float width, float height, SkImage *image, SkPaint *paint) {
    auto rate = width / height;

    SkRect dst(0, 0, 0, 0);
    auto imgW = (float) image->width();
    auto imgH = (float) image->height();
    double imgRate = imgW / imgH;
    float sw = width / imgW;
    float sh = height / imgH;
    if (imgRate > rate) {
        auto targetW = sh * imgW;
        dst.fBottom = height;
        auto d = (targetW - width) / 2;
        dst.fLeft = -d;
        dst.fRight = width + d;
    } else {
        auto targetH = sw * imgH;
        dst.fRight = width;
        auto d = (targetH - height) / 2;
        dst.fTop = -d;
        dst.fBottom = height + d;
    }
    canvas->drawImageRect(image, dst, SkSamplingOptions(), paint);
}
void VideoDrawer::drawImageContain(SkCanvas *canvas, float width, float height, SkImage *image, SkPaint *paint) {
    auto rate = width / height;

    SkRect dst(0, 0, 0, 0);
    auto imgW = (float) image->width();
    auto imgH = (float) image->height();
    double imgRate = imgW / imgH;
    float sw = width / imgW;
    float sh = height / imgH;
    if (imgRate > rate) {
        auto targetH = sw * imgH;
        dst.fRight = width;
        auto d = (height - targetH) / 2;
        dst.fTop = d;
        dst.fBottom = height - d;
    } else {
        auto targetW = sh * imgW;
        dst.fBottom = height;
        auto d = (width - targetW) / 2;
        dst.fLeft = d;
        dst.fRight = width - d;
    }
    canvas->drawImageRect(image, dst, SkSamplingOptions(), paint);
}

void VideoDrawer::drawImageStretch(SkCanvas *canvas, float width, float height, SkImage *image, SkPaint *paint) {
    SkRect dst(0, 0, width, height);
    canvas->drawImageRect(image, dst, SkSamplingOptions(), paint);
}

void VideoDrawer::drawImageCenter(SkCanvas *canvas, float width, float height, SkImage *image, SkPaint *paint) {
    auto imgW = (float) image->width();
    auto imgH = (float) image->height();
    SkRect dst(0, 0, imgW, imgH);
    auto dw = (width - imgW) / 2;
    auto dh = (height - imgH) / 2;
    dst.offset(dw, dh);
    canvas->drawImageRect(image, dst, SkSamplingOptions(), paint);
}

void VideoDrawer::drawImageRepeat(SkCanvas *canvas, float width, float height, SkImage *image, SkPaint *paint) {
    auto imgW = (float) image->width();
    auto imgH = (float) image->height();

    int countX = (int) ceil(width / imgW);
    int countY = (int) ceil(height / imgH);
    float endX, endY = 0;
    for (int i = 0; i < countY; ++i) {
        endX = 0;
        for (int j = 0; j < countX; ++j) {
            canvas->drawImage(image, endX, endY, SkSamplingOptions(), paint);
            endX += imgW;
        }
        endY += imgH;
    }
}

void VideoDrawer::drawImagePip(SkCanvas *canvas, float width, float height, SkImage *image, SkPaint *paint) {
    auto rate = width / height;
    auto imgRate = ((float) image->width()) / image->height();
    if (abs(rate - imgRate) < 1e-3) {
        VideoDrawer::drawImageStretch(canvas, width, height, image, nullptr);
        return;
    }
    drawImageClip(canvas, width, height, image, paint);
    drawImageContain(canvas, width, height, image);
}


void VideoDrawer::DrawImage(SkCanvas *canvas, float width, float height, SkImage *image, ContentFit fit, SkPaint *paint) {
    auto fn = drawFnMap.at(fit);
    if (fit == ContentFit::PIP)
        fn(canvas, width, height, image, &pipPaint);
    else
        fn(canvas, width, height, image, nullptr);
}

void VideoDrawer::SetFrame(VideoFrame *frame) {
    this->frame = frame;
}
