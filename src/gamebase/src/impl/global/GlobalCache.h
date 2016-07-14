/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/GLTexture.h>
#include "src/impl/graphics/TextureKey.h"
#include <json/value.h>
#include <unordered_map>

namespace gamebase { namespace impl {

struct GlobalCache {
    std::unordered_map<TextureKey, GLTexture, TextureKeyHash> textureCache;
    std::unordered_map<std::string, std::shared_ptr<Json::Value>> designCache;
};

extern GlobalCache g_cache;

} }
