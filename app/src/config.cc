#include <config.h>

#define UYAML_USE_STATIC

#include <file-utils.h>
#include <map>
#include <sstream>
#include <utility>
#include <uyaml/uyaml.h>

HWallpaperConfig config;

extern std::string appPath;

std::string configFile;


std::pair<str, str> line_get_key_value(const str &line) {
    auto index = line.find('=');
    if (index != str::npos)
        return std::make_pair(line.substr(0, index), line.substr(index + 1));
    return std::make_pair(line, TEXT(""));
}

str strTrim(const str &str, bool start = true, bool end = true) {
    if (str.empty())
        return TEXT("");
    int si = 0, ei = str.size() - 1;
    if (start)
        while (si <= ei && str[si] == ' ' || str[si] == '\r')
            si++;
    if (end)
        while (si <= ei && str[ei] == ' ' || str[ei] == '\r')
            ei--;
    return str.substr(si, ei - si + 1);
}


void initConfig() {
    configFile = appPath + "/config.yaml";
    if (!file_exists(configFile)) {
        file_create_empty(configFile);
    }

    size_t len;
    auto data = file_read(configFile, len);
    u8str u8text(data, data + len);
    free(data);

    auto text = u8str2str(u8text);

    UYAML::WNode yaml;
    try {
        yaml.Parse(text);
    } catch (std::exception &e) {
        MessageBoxA(nullptr, e.what(), "Parse config failed", MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY);
    }

    auto &update = *yaml[L"update"];
    config.update.checkOnStart = update[L"checkOnStart"]->asBool(true);

    auto &wallpaper = *yaml[L"wallpaper"];
    config.wallpaper.file = str2u8str(strTrim(wallpaper[L"file"]->asString()));
    config.wallpaper.fit = parse_content_fit(wallpaper[L"fit"]->asString());
    config.wallpaper.time = wallpaper[L"time"]->asFloat();
}

bool SaveConfig() {
    UYAML::WNode yaml;
    auto &update = *yaml[L"update"];
    (*update[L"checkOnStart"]) = config.update.checkOnStart;
    auto &wallpaper = *yaml[L"wallpaper"];
    (*wallpaper[L"file"]) = u8str2str(config.wallpaper.file);
    (*wallpaper[L"fit"]) = content_fit_to_str(config.wallpaper.fit);
    (*wallpaper[L"time"]) = config.wallpaper.time;

    u8str data = str2u8str(yaml.Dump<1>(L"\n"));
    file_write(configFile, (char *) data.data(), data.length());
    return true;
}

ContentFit parse_content_fit(const str &fit) {
    if (fit == TEXT("clip")) {
        return ContentFit::CLIP;
    }
    if (fit == TEXT("contain")) {
        return ContentFit::CONTAIN;
    }
    if (fit == TEXT("stretch")) {
        return ContentFit::STRETCH;
    }
    if (fit == TEXT("center")) {
        return ContentFit::CENTER;
    }
    if (fit == TEXT("repeat")) {
        return ContentFit::REPEAT;
    }
    if (fit == TEXT("pip")) {
        return ContentFit::PIP;
    }
    return ContentFit::PIP;
}

str content_fit_to_str(ContentFit fit) {
    switch (fit) {
        case ContentFit::CLIP:
            return TEXT("clip");
        case ContentFit::CONTAIN:
            return TEXT("contain");
        case ContentFit::STRETCH:
            return TEXT("stretch");
        case ContentFit::CENTER:
            return TEXT("center");
        case ContentFit::REPEAT:
            return TEXT("repeat");
        case ContentFit::PIP:
            return TEXT("pip");
    }
    return TEXT("pip");
}
