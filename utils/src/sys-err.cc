#include "sys-err.h"

str get_error_message(DWORD code) {
    auto *buffer = new WCHAR[4096];
    if (!FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM,
            nullptr,
            code,
            0,
            buffer,
            4096,
            nullptr))
        return TEXT("");
    str r = buffer;
    delete[] buffer;
    return r;
}

void error_message(DWORD code) {
    if (!code)
        return;
    auto msg = get_error_message(code);
    error_not_throw(msg.empty() ? TEXT("未知错误") : msg);
}
