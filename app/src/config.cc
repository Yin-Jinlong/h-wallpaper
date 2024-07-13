#include <config.h>

#include <file-utils.h>

HWallpaperConfig config;

extern std::string appPath;

std::string configFile;

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
    setValue(&config.wallpaper.time, getSubNodeOrNew(wallpaper, "time"));
}

bool SaveConfig() {
    auto yaml = YAML::Node();
    auto wallpaper = yaml["wallpaper"];
    wallpaper["file"] = config.wallpaper.file;
    wallpaper["time"] = config.wallpaper.time;
    return file_write(configFile, YAML::Dump(yaml));
}
