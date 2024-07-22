#pragma once

#include <pre.h>

enum WallpaperType {
    WALLPAPER_TYPE_NONE,
    WALLPAPER_TYPE_COLOR,
    WALLPAPER_TYPE_IMAGE,
    WALLPAPER_TYPE_VIDEO,
};

class Wallpaper {
private:
    WallpaperType type;

protected:

    int width = 0, height = 0;

    explicit Wallpaper(WallpaperType type);

    ~Wallpaper();

    void requestRedraw() const;

public:
    USE_RET WallpaperType Type() const;

    virtual void draw(HDC mdc) {

    }

    virtual void Pause() {

    }

    virtual void Resume() {

    }

    virtual void Restart() {

    }

    virtual bool Paused() const {
        return false;
    };

    virtual void SetSize(int width, int height);
};
