#include <image-util.h>

bool SkBitmapSetRgb888Pixels(SkBitmap *bitmap, size_t width, size_t height, RGBColor *pixels, byte alpha) {
    if (!bitmap)
        return false;
    if (bitmap->colorType() != kRGBA_8888_SkColorType)
        error("SkBitmapSetRgb888Pixels: color type must be kRGBA_8888_SkColorType");

    auto pixmap = bitmap->pixmap();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint32_t *pixel = pixmap.writable_addr32(x, y);
            auto rgb = pixels[y * width + x];
            *pixel = 255 << 24 | 255 << 16 | 0 << 8 | alpha;
        }
    }
    return true;
}

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
