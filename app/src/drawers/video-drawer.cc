#include <drawers/video-drawer.h>
#include "image-util.h"

extern HWallpaperConfig config;

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
    DrawImage(canvas, frameBitmap.asImage().get(), config.wallpaper.fit);

    free(frame->data);
    frame->data = nullptr;
    return true;
}

void VideoDrawer::DrawImage(SkCanvas *canvas, SkImage *image, ContentFit fit, SkPaint *paint) {

    auto imgW = (float) image->width();
    auto imgH = (float) image->height();
    double imgRate = imgW / imgH;
    float sw = width / imgW;
    float sh = height / imgH;

    SkRect dst(0, 0, 0, 0);
    switch (fit) {
        case ContentFit::CLIP: {
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
            break;
        }
        case ContentFit::CONTAIN: {
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
            break;
        }
        case ContentFit::STRETCH:
            dst.fRight = width;
            dst.fBottom = height;
            break;
        case ContentFit::CENTER: {
            dst.setWH(imgW, imgH);
            auto dw = (width - imgW) / 2;
            auto dh = (height - imgH) / 2;
            dst.offset(dw, dh);
            break;
        }
        case ContentFit::REPEAT: {
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
            break;
        }
        case ContentFit::PIP: {
            DrawImage(canvas, image, ContentFit::CLIP, &pipPaint);
            DrawImage(canvas, image, ContentFit::CONTAIN);
            break;
        }
        default:
            return;
    }

    canvas->drawImageRect(image, dst, SkSamplingOptions(), paint);
}

void VideoDrawer::SetFrame(VideoFrame *frame) {
    this->frame = frame;
}
