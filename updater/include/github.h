#pragma once

#include <pre.h>
#include <vector>
#include <json/json.h>
#include <json.h>

struct GithubUser {
    str login;
    uint32_t id = 0;
    str node_id;
    str avatar_url;
    str gravatar_id;
    str url;
    str html_url;
    str followers_url;
    str following_url;
    str gists_url;
    str starred_url;
    str subscriptions_url;
    str organizations_url;
    str repos_url;
    str events_url;
    str received_events_url;
    str type;
    bool site_admin = false;
};

struct GithubAsset {
    str url;
    uint32_t id = 0;
    str node_id;
    str name;
    str label;
    GithubUser uploader;
    str content_type;
    str state;
    uint64_t size = 0;
    uint64_t download_count = 0;
    str created_at;
    str updated_at;
    str browser_download_url;
};

struct GithubReleaseInfo {
    str url;
    str assets_url;
    str upload_url;
    str html_url;
    uint32_t id = 0;
    GithubUser author;
    str node_id;
    str tag_name;
    str target_commitish;
    str name;
    bool draft = false;
    bool prerelease = false;
    str created_at;
    str published_at;
    std::vector<GithubAsset> assets;
    str tarball_url;
    str zipball_url;
    str body;
};


template<>
GithubUser Json::Value::as<GithubUser>() const {
    if (isNull()) {
        return {};
    }
    return GithubUser{
            .login = getJsonStr(this, u8"login"),
            .id = static_cast<uint32_t>(getJsonNumber(this, u8"id")),
            .node_id = getJsonStr(this, u8"node_id"),
            .avatar_url = getJsonStr(this, u8"avatar_url"),
            .gravatar_id = getJsonStr(this, u8"gravatar_id"),
            .url = getJsonStr(this, u8"url"),
            .html_url = getJsonStr(this, u8"html_url"),
            .followers_url = getJsonStr(this, u8"followers_url"),
            .following_url = getJsonStr(this, u8"following_url"),
            .gists_url = getJsonStr(this, u8"gists_url"),
            .starred_url = getJsonStr(this, u8"starred_url"),
            .subscriptions_url = getJsonStr(this, u8"subscriptions_url"),
            .organizations_url = getJsonStr(this, u8"organizations_url"),
            .repos_url = getJsonStr(this, u8"repos_url"),
            .events_url = getJsonStr(this, u8"events_url"),
            .received_events_url = getJsonStr(this, u8"received_events_url"),
            .type = getJsonStr(this, u8"type"),
            .site_admin = getJsonBool(this, u8"site_admin"),
    };
}


template<>
std::vector<GithubAsset> Json::Value::as<std::vector<GithubAsset>>() const {
    if (isNull() || !isArray()) {
        return {};
    }
    std::vector<GithubAsset> assets;
    for (auto &asset: *this) {
        GithubAsset a;
        a.url = getJsonStr(&asset, u8"url");
        a.id = getJsonNumber(&asset, u8"id");
        a.node_id = getJsonStr(&asset, u8"node_id");
        a.name = getJsonStr(&asset, u8"name");
        a.label = getJsonStr(&asset, u8"label");
        a.uploader = getJsonValue(&asset, u8"uploader").as<GithubUser>();
        a.content_type = getJsonStr(&asset, u8"content_type");
        a.state = getJsonStr(&asset, u8"state");
        a.size = getJsonNumber(&asset, u8"size");
        a.download_count = getJsonNumber(&asset, u8"download_count");
        a.created_at = getJsonStr(&asset, u8"created_at");
        a.updated_at = getJsonStr(&asset, u8"updated_at");
        a.browser_download_url = getJsonStr(&asset, u8"browser_download_url");
        assets.push_back(a);
    }
    return assets;
}


template<>
GithubReleaseInfo Json::Value::as<GithubReleaseInfo>() const {
    if (isNull()) {
        return {};
    }
    GithubReleaseInfo info;
    info.url = getJsonStr(this, u8"url");
    info.assets_url = getJsonStr(this, u8"assets_url");
    info.upload_url = getJsonStr(this, u8"upload_url");
    info.html_url = getJsonStr(this, u8"html_url");
    info.id = getJsonNumber(this, u8"id");
    info.author = getJsonValue(this, u8"author").as<GithubUser>();
    info.node_id = getJsonStr(this, u8"node_id");
    info.tag_name = getJsonStr(this, u8"tag_name");
    info.target_commitish = getJsonStr(this, u8"target_commitish");
    info.name = getJsonStr(this, u8"name");
    info.draft = getJsonBool(this, u8"draft");
    info.prerelease = getJsonBool(this, u8"prerelease");
    info.created_at = getJsonStr(this, u8"created_at");
    info.published_at = getJsonStr(this, u8"published_at");
    info.assets = getJsonValue(this, u8"assets").as<std::vector<GithubAsset>>();
    info.tarball_url = getJsonStr(this, u8"tarball_url");
    info.zipball_url = getJsonStr(this, u8"zipball_url");
    info.body = getJsonStr(this, u8"body");
    return info;
}
