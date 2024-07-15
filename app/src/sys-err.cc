#include "sys-err.h"

std::string get_error_message(DWORD code) {
    CHAR *buffer = new CHAR[4096];
    if (!FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM,
            nullptr,
            code,
            0,
            buffer,
            4096,
            nullptr
    ))
        return "";
    std::string r = buffer;
    delete[] buffer;
    return r;
}

void error_message(DWORD code) {
    if (!code)
        return;
    auto msg = get_error_message(code);
    error_not_throw(msg.empty() ? "未知错误" : msg);
}
