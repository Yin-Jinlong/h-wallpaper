#pragma once

#include <pre.h>

#include "video-decoder.h"

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

    /**
     * 视频解码器
     */
    std::atomic<VideoDecoder *> decoderPtr = nullptr;

    /**
     * 窗口宽高
     */
    int width = 0, height = 0;

public:

    /**
     * 查找已存在的壁纸窗口
     *
     * @return 窗口句柄
     */
    static HWND FindExist();

    /**
     * 当前帧时间
     */
    double frameTime = 0;
    /**
     * 当前时间
     */
    double nowTime = 0;
    /**
     * 上一帧时间，用于更新当前时间
     */
    double lastTime = 0;

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
     * 设置壁纸
     *
     * @param file 视频文件
     */
    void SetVideo(std::string file,bool save=true);

    /**
     * 绘制壁纸
     *
     * @param hdc 绘图设备
     */
    void paint(HDC hdc);

    /**
     * 解码器可用
     *
     * @return 是否可用
     */
    bool decoderAvailable();

    /**
     * 第一帧是否已加载
     * @return 是否已加载
     */
    bool firstFrameLoaded();

    /**
     * 设置窗口大小
     *
     * @param width 宽度
     * @param height 高度
     */
    void SetSize(int width, int height);

    /**
     * 解码器是否已暂停
     *
     * @return 是否已暂停
     */
    bool decoderPaused();

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
};
