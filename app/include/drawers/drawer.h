#pragma once

#include <pre.h>

#include <include/core/SkBitmap.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkColor.h>
#include <include/core/SkPaint.h>
#include <include/core/SkSurface.h>

#include "config.h"

class Drawer {
protected:

    double rate = 0;
    float width = 0, height = 0;

public:

    Drawer();

    ~Drawer();

    /**
     * @brief 绘制帧
     * @param canvas 画布，不能为空
     * @param frame 帧
     * @return 是否绘制了
     */
    virtual bool Draw(SkCanvas *canvas) {
        return false;
    }

    void SetSize(int width, int height);

};
