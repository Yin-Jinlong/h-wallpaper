#include <drawers/drawer.h>

Drawer::Drawer() = default;

Drawer::~Drawer() = default;

void Drawer::SetSize(int width, int height) {
    this->width = (float) width;
    this->height = (float) height;
    rate = ((double) width) / height;
}
