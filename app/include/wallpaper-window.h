#pragma once

#include <pre.h>

#include "video-decoder.h"
#include "drawers/drawer.h"
#include "wallpapers/wallpaper.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
}

/**
 * 壁纸窗口
 *
 * @author YJL
 */
class WallpaperWindow {

private:

    HWND hWnd;

    BITMAPINFO bmi{0};

    std::atomic<Wallpaper *> wallpaperPtr = nullptr;

    sk_sp<SkSurface> surface;

    /**
     * 窗口宽高
     */
    int width = 0, height = 0;

    void drawToHdc(HDC hdc);

public:

    /**
     * 查找已存在的壁纸窗口
     *
     * @return 窗口句柄
     */
    static HWND FindExist();

    explicit WallpaperWindow(HINSTANCE hInstance);

    ~WallpaperWindow();

    /**
     * 显示窗口
     */
    void Show();

    /**
     * 设置窗口到桌面
     */
    void SetToDesktop();

    /**
     * 绘制壁纸
     *
     * @param hdc 绘图设备
     */
    void paint(HDC hdc);

    /**
     * 设置窗口大小
     *
     * @param width 宽度
     * @param height 高度
     */
    void SetSize(int width, int height);

    /**
     * 暂停
     */
    void pause();

    /**
     * 恢复
     */
    void resume();

    /**
     * 发送查询最大化消息
     */
    void PostQueryMaximized();

    /**
     * 获取窗口宽度
     *
     * @return 宽度
     */
    int GetWidth() const;

    /**
     * 获取窗口高度
     *
     * @return 高度
     */
    int GetHeight() const;

    /**
     * 重绘
     */
    void Redraw();

    /**
     * 是否已暂停
     *
     * @return 是否已暂停
     */
    bool Paused() const;

    /**
     * 获取壁纸
     *
     * @return 壁纸
     */
    Wallpaper *GetWallpaper();
};
