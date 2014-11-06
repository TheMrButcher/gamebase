#pragma once

#include <string>

namespace gamebase {

struct Config {
    Config();

    std::string shadersPath;
    std::string imagesPath;
};

void loadConfig(const std::string& fileName);
const Config& config();

}
