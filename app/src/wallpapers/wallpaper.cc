#include <wallpapers/wallpaper.h>
#include <wallpaper-window.h>

extern WallpaperWindow *wallpaperWindow;

Wallpaper::Wallpaper(WallpaperType type) : type(type) {

}

Wallpaper::~Wallpaper() = default;


void Wallpaper::requestRedraw() const {
    wallpaperWindow->Redraw();
}

WallpaperType Wallpaper::Type() const {
    return type;
}

void Wallpaper::SetSize(int width, int height) {
    this->width = width;
    this->height = height;
}

