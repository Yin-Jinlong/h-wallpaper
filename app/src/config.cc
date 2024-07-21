#include <config.h>

#include <file-utils.h>

HWallpaperConfig config;

extern std::string appPath;

std::string configFile;

template<>
struct YAML::convert<ContentFit> {
    static YAML::Node encode(const ContentFit &rhs) {
        switch (rhs) {
            case ContentFit::CONTAIN:
                return YAML::Node("contain");
            case ContentFit::STRETCH:
                return YAML::Node("stretch");
            case ContentFit::CENTER:
                return YAML::Node("center");
            case ContentFit::REPEAT:
                return YAML::Node("repeat");
            case ContentFit::PIP:
                return YAML::Node("pip");
            default:
                return YAML::Node("clip");
        }
    }

    static bool decode(const YAML::Node &node, ContentFit &rhs) {
        if (node.IsScalar()) {
            const std::string &value = node.Scalar();
            if (value == "clip")
                rhs = ContentFit::CLIP;
            else if (value == "contain")
                rhs = ContentFit::CONTAIN;
            else if (value == "stretch")
                rhs = ContentFit::STRETCH;
            else if (value == "center")
                rhs = ContentFit::CENTER;
            else if (value == "repeat")
                rhs = ContentFit::REPEAT;
            else if (value == "pip")
                rhs = ContentFit::PIP;
            else
                rhs = ContentFit::CLIP;
            return true;
        }
        return false;
    }
};

template<>
struct YAML::convert<u8str> {
    static YAML::Node encode(const u8str &rhs) {
        return YAML::Node(u8str2string(rhs));
    }

    static bool decode(const YAML::Node &node, u8str &rhs) {
        if (node.IsScalar()) {
            const std::string &value = node.Scalar();
            rhs = string2u8string(value);
            return true;
        }
        return false;
    }
};

template<typename T>
bool setValue(T *configPtr, const YAML::Node &key) {
    try {
        if (key.IsScalar()) {
            (*configPtr) = key.as<T>();
            return true;
        }
    } catch (...) {
    }
    return false;
}

YAML::Node getSubNodeOrNew(const YAML::Node &node, const std::string &key) {
    if (node.IsDefined()) {
        return node.IsMap() ? node[key] : YAML::Node();
    }
    return YAML::Node();
}

void initConfig() {
    configFile = appPath + "/config.yaml";
    if (!file_exists(configFile)) {
        file_create_empty(configFile);
    }

    std::ifstream in(configFile, std::ios::in, std::ios::binary);
    if (!in.is_open()) {
        return;
    }
    in.seekg(0, std::ios::end);
    auto len = in.tellg();
    in.seekg(0, std::ios::beg);
    auto data = new char[len];
    in.read(data, len);
    in.close();

    auto yaml = YAML::Load(u8str2string(reinterpret_cast<char8_t *>(data)));
    delete[] data;

    auto update = getSubNodeOrNew(yaml, "update");
    setValue(&config.update.checkOnStart, getSubNodeOrNew(update, "checkOnStart"));

    auto wallpaper = getSubNodeOrNew(yaml, "wallpaper");

    setValue(&config.wallpaper.file, getSubNodeOrNew(wallpaper, "file"));
    setValue(&config.wallpaper.fit, getSubNodeOrNew(wallpaper, "fit"));
    setValue(&config.wallpaper.time, getSubNodeOrNew(wallpaper, "time"));
}

bool SaveConfig() {
    auto yaml = YAML::Node();
    auto update = yaml["update"];
    update["checkOnStart"] = config.update.checkOnStart;

    auto wallpaper = yaml["wallpaper"];
    wallpaper["file"] = config.wallpaper.file;
    wallpaper["fit"] = config.wallpaper.fit;
    wallpaper["time"] = config.wallpaper.time;
    auto dump = string2u8string(YAML::Dump(yaml));
    auto size = strlen(reinterpret_cast<char *>(dump.data()));
    return file_write(configFile, dump.data(), size);
}
