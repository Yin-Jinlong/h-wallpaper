#include <string-table.h>

std::wstring GetStr(int id) {
    WCHAR buf[1024];
    return LoadString(GetModuleHandle(nullptr), id, buf, 1024) ? buf : L"";
}
