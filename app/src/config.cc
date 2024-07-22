#include <config.h>

#include <file-utils.h>
#include <utility>
#include <vector>
#include <map>
#include <sstream>

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
        while (si <= ei && str[si] == ' ')
            si++;
    if (end)
        while (si <= ei && str[ei] == ' ')
            ei--;
    return str.substr(si, ei - si + 1);
}


void initConfig() {
    configFile = appPath + "/config";
    if (!file_exists(configFile)) {
        file_create_empty(configFile);
    }

    size_t len;
    auto data = file_read(configFile, len);
    u8str u8text(data, data + len);
    free(data);

    std::wistringstream stream(u8str2str(u8text));
    str line;
    int lineNumber = 0;

    std::map<str, str> map;

    while (std::getline(stream, line)) {
        lineNumber++;
        if (line.ends_with('\r'))
            line = line.substr(0, line.size() - 1);
        line = strTrim(line);
        if (line[0] == '#')
            continue;
        auto kv = line_get_key_value(line);
        auto key = strTrim(kv.first);
        if (key.empty())
            continue;
        auto v = strTrim(kv.second);
        // 转义字符串
        if (v[0] == '"') {
            if (v.ends_with('"'))
                v = v.substr(1, v.size() - 2);
        }
        map[key] = v;
    }

    config.update.checkOnStart = map[TEXT("update.checkOnStart")] == TEXT("true");
    config.wallpaper.file = str2u8str(map[TEXT("wallpaper.file")]);
    config.wallpaper.fit = parse_content_fit(map[TEXT("wallpaper.fit")]);
    auto ts = map[TEXT("wallpaper.time")];
    if (ts.empty())
        ts = TEXT("0");
    else {
        int dotCount = 0;
        for (auto c: ts) {
            if (c >= '0' && c <= '9')
                continue;
            if (c == '.') {
                dotCount++;
                if (dotCount > 1)
                    return;
            }
        }
    }
    config.wallpaper.time = std::stoi(ts);
}

bool SaveConfig() {
    std::wostringstream out;
    out << TEXT("update.checkOnStart=") << (config.update.checkOnStart ? TEXT("true") : TEXT("false")) << std::endl;
    out << TEXT("wallpaper.file=") << u8str2str(config.wallpaper.file) << std::endl;
    out << TEXT("wallpaper.fit=") << content_fit_to_str(config.wallpaper.fit) << std::endl;
    out << TEXT("wallpaper.time=") << config.wallpaper.time << std::endl;

    u8str data = str2u8str(out.str());
    auto byteCount = strlen(reinterpret_cast<const char *>(data.c_str()));
    file_write(configFile, (char *) data.c_str(), byteCount);
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
    return ContentFit::CLIP;
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
    return TEXT("clip");
}
