#pragma once

#include <pre.h>

#include <include/core/SkBitmap.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkColor.h>
#include <include/core/SkPaint.h>
#include <include/core/SkSurface.h>

#include "config.h"

/**
 * @brief 绘制器，绘制到Canvas上
 */
class Drawer {
protected:

    /**
     * @brief 宽高比
     */
    double rate = 0;
    /**
     * @brief 宽度
     */
    float width = 0;
    /**
     * @brief 高度
     */
    float height = 0;

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

    /**
     * @brief 设置尺寸
     * @param width 宽度
     * @param height 高度
     */
    void SetSize(int width, int height);

};
