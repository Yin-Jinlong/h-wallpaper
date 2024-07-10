#pragma once

#include <pre.h>

#include <yaml-cpp/yaml.h>

extern YAML::Node config;

/**
 * @brief 初始化配置
 *
 * @author YJL
 */
void initConfig();

/**
 * @brief 保存配置
 * @return 是否成功保存
 *
 * @author YJL
 */
bool SaveConfig();
