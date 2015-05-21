#pragma once

#include <gamebase/graphics/GraphicsMode.h>
#include <string>

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
};

void loadConfig(const std::string& fileName);
const Config& config();

}
