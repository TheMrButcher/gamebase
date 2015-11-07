#pragma once

#include <gamebase/graphics/Texture.h>
#include <unordered_map>

namespace gamebase {

struct GlobalCache {
    std::unordered_map<std::string, Texture> textureCache;
};

extern GlobalCache g_cache;

}
