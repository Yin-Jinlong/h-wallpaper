#include <image-util.h>

RGBColor *SkImageGetRgb888Pixels(SkImage *image) {
    SkPixmap pixmap;
    if (!image || !image->peekPixels(&pixmap) || image->colorType() != kRGBA_8888_SkColorType)
        return nullptr;
    auto width = pixmap.width();
    auto height = pixmap.height();
    auto res = (RGBColor *) malloc(width * height * sizeof(RGBColor));
    for (int i = 0; i < width * height; i++) {
        auto pixel = (RGBAColor *) pixmap.addr32(i % width, i / width);
        auto rgb = &res[i];
        rgb->r = pixel->r;
        rgb->g = pixel->g;
        rgb->b = pixel->b;
    }
    return res;
}

BGRColor *SkImageGetBgr888Pixels(SkImage *image) {
    SkPixmap pixmap;
    if (!image || !image->peekPixels(&pixmap) || image->colorType() != kRGBA_8888_SkColorType)
        return nullptr;
    auto width = pixmap.width();
    auto height = pixmap.height();
    auto res = (BGRColor *) malloc(width * height * sizeof(BGRColor));
    for (int i = 0; i < width * height; i++) {
        auto pixel = (RGBAColor *) pixmap.addr32(i % width, i / width);
        auto bgr = &res[i];
        bgr->r = pixel->r;
        bgr->g = pixel->g;
        bgr->b = pixel->b;
    }
    return res;
}
