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
        {"Accept", " application/vnd.github+json"}};
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

int main(int argc, char *argv[]) {
    bool onlyNew = false;
    if (argc > 1) {
        str option = string2str(argv[1]);
        if (option == TEXT("--only-new")) {
            onlyNew = true;
        }
    }

    size_t len;
    auto data = file_read("VERSION", len);
    if (!data) {
        return 1;
    }
    u8str text = u8str(data, data + len);

    free(data);

    if (text.empty()) {
        return 1;
    }

    auto version = latestVersion();
    if (version.empty()) {
        if (!onlyNew) {
            MessageBox(nullptr,
                       TEXT("无法获取最新版本"),
                       APP_NAME,
                       MB_OK);
        }
        return 404;
    }

    auto nowVersion = u8str2str(text);
    if (version > nowVersion) {
        auto btn = MessageBox(nullptr,
                              std::format(TEXT("发现新版本：{}\n当前：{}\n立即更新？"), version, nowVersion).c_str(),
                              APP_NAME,
                              MB_YESNOCANCEL);
        if (btn == IDYES) {
            ShellExecute(
                nullptr,
                TEXT("open"),
                TEXT("https://github.com/Yin-Jinlong/h-wallpaper/releases/latest"),
                nullptr,
                nullptr,
                SW_SHOW);
        }
    } else if (!onlyNew) {
        MessageBox(nullptr,
                   std::format(TEXT("当前版本：{}\n已是最新版本"), nowVersion).c_str(),
                   APP_NAME,
                   MB_OK);
    }
    return 0;
}
