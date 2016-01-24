#pragma once

#include <gamebase/core/Core.h>
#include <gamebase/graphics/GraphicsMode.h>
#include <string>
#include <unordered_map>

namespace gamebase {

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

}
