#pragma once

#include <pre.h>

#include <include/core/SkBitmap.h>
#include <include/core/SkImage.h>

#include <color.h>

RGBColor *SkImageGetRgb888Pixels(SkImage *image);

BGRColor *SkImageGetBgr888Pixels(SkImage *image);
