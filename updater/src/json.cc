#include <json.h>

template<>
str Json::Value::as<str>() const {
    auto s = asString();
    return u8str2str(stringAsU8str(s));
}

Json::Value getJsonValue(const Json::Value *v, u8str key) {
    return v->get(u8AsString(std::move(key)), Json::Value());
}

str getJsonStr(const Json::Value *v, u8str key) {
    static Json::Value NullString("");
    return v->get(u8AsString(std::move(key)), NullString).as<str>();
}

int32_t getJsonNumber(const Json::Value *v, u8str key) {
    return v->get(u8AsString(std::move(key)), 0).as<int32_t>();
}

bool getJsonBool(const Json::Value *v, u8str key) {
    return v->get(u8AsString(std::move(key)), 0).as<bool>();
}
