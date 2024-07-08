#include "wnd-utils.h"

bool HasWindowMaximized() {
    bool result = false;
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        if (IsZoomed(hwnd)) {
            *reinterpret_cast<bool *>(lParam) = true;
            return false;
        }
        return true;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}
