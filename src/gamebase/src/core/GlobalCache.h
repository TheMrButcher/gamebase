#pragma once

#include <gamebase/graphics/Texture.h>
#include <json/value.h>
#include <unordered_map>

namespace gamebase {

struct GlobalCache {
    std::unordered_map<std::string, Texture> textureCache;
    std::unordered_map<std::string, std::shared_ptr<Json::Value>> designCache;
};

extern GlobalCache g_cache;

}
