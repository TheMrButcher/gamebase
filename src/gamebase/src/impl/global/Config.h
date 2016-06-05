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
    std::string windowName;
    int width;
    int height;

    std::string configSource;
    Dictionary dict;
};

const Config& config();

} }
