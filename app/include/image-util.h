#pragma once

#include <pre.h>

#include <include/core/SkBitmap.h>
#include <include/core/SkImage.h>

#include <color.h>

bool SkBitmapSetRgb888Pixels(SkBitmap *bitmap, size_t width, size_t height, RGBColor *pixels, byte alpha = 0xFF);

RGBColor *SkImageGetRgb888Pixels(SkImage *image);

BGRColor *SkImageGetBgr888Pixels(SkImage *image);
