#pragma once

#include <pre.h>
#include <include/core/SkCanvas.h>

/**
 * @brief 壁纸类型
 */
enum WallpaperType {
    /**
     * @brief 无
     */
    WALLPAPER_TYPE_NONE,
    /**
     * @brief 颜色（纯色，渐变等）
     */
    WALLPAPER_TYPE_COLOR,
    /**
     * @brief 图片
     */
    WALLPAPER_TYPE_IMAGE,
    /**
     * @brief 视频
     */
    WALLPAPER_TYPE_VIDEO,
};

/**
 * @brief 抽象壁纸类
 *
 * @author YJL
 */
class Wallpaper {
private:
    /**
     * @brief 壁纸类型
     * @see WallpaperType
     */
    WallpaperType type;

protected:

    /**
     * @brief 壁纸宽（显示）
     */
    int width = 0;
    /**
     * @brief 壁纸高（显示）
     */
    int height = 0;

    explicit Wallpaper(WallpaperType type);

    ~Wallpaper();

    /**
     * @brief 请求重绘
     */
    void requestRedraw() const;

public:
    /**
     * @brief 获取壁纸类型
     * @return 壁纸类型
     * @see WallpaperType
     */
    USE_RET WallpaperType Type() const;

    /**
     * @brief 绘制
     * @param canvas 画布
     */
    virtual void draw(SkCanvas *canvas) {

    }

    /**
     * @brief 暂停
     */
    virtual void Pause() {

    }

    /**
     * @brief 恢复
     */
    virtual void Resume() {

    }

    /**
     * @brief 重新开始
     */
    virtual void Restart() {

    }

    /**
     * @brief 是否暂停
     * @return 是否暂停
     */
    virtual bool Paused() const {
        return false;
    };

    /**
     * @brief 设置壁纸宽高
     * @param width 壁纸宽（显示）
     * @param height 壁纸高（显示）
     */
    virtual void SetSize(int width, int height);
};
