/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/app/Config.h>
#include <gamebase/tools/Size.h>
#include <gamebase/impl/graphics/GraphicsMode.h>
#include <boost/optional.hpp>
#include <string>
#include <unordered_map>

namespace gamebase { namespace impl {

struct Config {
    Config();

    std::string shadersPath;
    std::string imagesPath;
    std::string soundsPath;
    std::string musicPath;
    std::vector<std::string> fontsPath;
    std::string designPath;

    GraphicsMode::Enum mode;
    std::string windowTitle;
    Size windowSize;
    boost::optional<Size> minWindowSize;
    boost::optional<Size> maxWindowSize;
    bool showConsole;

    std::string configSource;
    Dictionary dict;
};

const Config& config();

} }
