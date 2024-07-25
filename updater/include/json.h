#pragma once

#include <json/json.h>
#include <pre.h>

#include "updater-str-util.h"

Json::Value getJsonValue(const Json::Value *v, u8str key);

str getJsonStr(const Json::Value *v, u8str key);

int32_t getJsonNumber(const Json::Value *v, u8str key);

bool getJsonBool(const Json::Value *v, u8str key);