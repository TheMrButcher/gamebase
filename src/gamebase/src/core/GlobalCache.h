#pragma once

#include <gamebase/graphics/GLTexture.h>
#include "src/graphics/TextureKey.h"
#include <json/value.h>
#include <unordered_map>

namespace gamebase {

struct GlobalCache {
    std::unordered_map<TextureKey, GLTexture, TextureKeyHash> textureCache;
    std::unordered_map<std::string, std::shared_ptr<Json::Value>> designCache;
};

extern GlobalCache g_cache;

}
