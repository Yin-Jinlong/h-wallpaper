#include <config.h>

#include <file-utils.h>

YAML::Node config;

extern std::string appPath;

std::string configFile;

void initConfig() {
    configFile = appPath + "/config.yaml";
    if (!file_exists(configFile)) {
        file_create_empty(configFile);
    }
    config = YAML::LoadFile(configFile);
}

bool SaveConfig() {
    return file_write(configFile, YAML::Dump(config));
}
