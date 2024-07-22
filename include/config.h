#pragma once

#include <pre.h>


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

ContentFit parse_content_fit(const str &fit);

str content_fit_to_str(ContentFit fit);

struct HWallpaperConfig {
    struct {
        bool checkOnStart = true;
    } update;
    struct {
        u8str file;
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
