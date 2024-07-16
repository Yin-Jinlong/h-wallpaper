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
            else
                rhs = ContentFit::CLIP;
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
    return node.IsMap() ? node[key] : YAML::Node();
}

void initConfig() {
    configFile = appPath + "/config.yaml";
    if (!file_exists(configFile)) {
        file_create_empty(configFile);
    }
    auto yaml = YAML::LoadFile(configFile);
    auto wallpaper = getSubNodeOrNew(yaml, "wallpaper");

    setValue(&config.wallpaper.file, getSubNodeOrNew(wallpaper, "file"));
    setValue(&config.wallpaper.fit, getSubNodeOrNew(wallpaper, "fit"));
    setValue(&config.wallpaper.time, getSubNodeOrNew(wallpaper, "time"));
}

bool SaveConfig() {
    auto yaml = YAML::Node();
    auto wallpaper = yaml["wallpaper"];
    wallpaper["file"] = config.wallpaper.file;
    wallpaper["fit"] = config.wallpaper.fit;
    wallpaper["time"] = config.wallpaper.time;
    return file_write(configFile, YAML::Dump(yaml));
}
