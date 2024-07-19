#pragma once

#include <pre.h>

#include <yaml-cpp/yaml.h>

enum class ContentFit {
    CLIP,
    CONTAIN,
    STRETCH,
    CENTER,
    REPEAT,
    /**
     * 画中画
     */
    PIP,
};

struct HWallpaperConfig {
    struct {
        std::u8string file;
        ContentFit fit;
        double time;
    } wallpaper;
};

extern HWallpaperConfig config;

/**
 * @brief 初始化配置
 *
 * @author YJL
 */
void initConfig();

/**
 * @brief 保存配置
 * @return 是否成功保存
 *
 * @author YJL
 */
bool SaveConfig();
