#pragma once

#include <pre.h>

#include <yaml-cpp/yaml.h>

extern YAML::Node config;

void initConfig();

bool SaveConfig();
