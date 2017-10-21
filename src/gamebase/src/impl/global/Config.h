/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/app/Config.h>
#include <gamebase/impl/graphics/GraphicsMode.h>
#include <string>
#include <unordered_map>

namespace gamebase { namespace impl {

struct Config {
    Config();

    std::string shadersPath;
    std::string imagesPath;
    std::string fontsPath;
    std::string designPath;

    GraphicsMode::Enum mode;
    std::string windowTitle;
    int width;
    int height;

    std::string configSource;
    Dictionary dict;
};

const Config& config();

} }
