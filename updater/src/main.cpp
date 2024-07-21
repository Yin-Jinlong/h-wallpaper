#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <httplib.h>
#include <github.h>
#include "file-utils.h"

#include <wrl/module.h>
#include "DesktopNotificationManagerCompat.h"
#include <NotificationActivationCallback.h>
#include <windows.ui.notifications.h>

using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::UI::Notifications;
using namespace Microsoft::WRL;

using namespace std;


str latestVersion() {
    httplib::Client api("https://api.github.com");

    httplib::Headers headers = {
            {"Accept", " application/vnd.github+json"}
    };
    auto res = api.Get("/repos/Yin-Jinlong/h-wallpaper/releases/latest");
    Json::Reader reader;
    Json::Value root;
    reader.parse(res->body, root);
    if (res->status != 200)
        return TEXT("");
    auto info = root.as<GithubReleaseInfo>();
    if (info.tag_name.empty())
        return TEXT("");
    return info.tag_name.substr(1);
}

int main() {
    auto data = file_read("VERSION");
    if (!data) {
        return 1;
    }
    u8str text = u8str((char8_t *) data);
    free(data);

    if (text.empty()) {
        return 1;
    }

    auto version = latestVersion();
    if (version.empty())
        return 404;

    auto nowVersion = u8str2str(text);
    if (version > nowVersion) {
        auto btn = MessageBox(nullptr, TEXT("发现新版本，立即更新？"), TEXT(APP_NAME), MB_YESNOCANCEL);
        if (btn == IDYES) {
            ShellExecute(
                    nullptr,
                    TEXT("open"),
                    TEXT("https://github.com/Yin-Jinlong/h-wallpaper/releases/latest"),
                    nullptr,
                    nullptr,
                    SW_SHOW);
        }
    }
    return 0;
}
