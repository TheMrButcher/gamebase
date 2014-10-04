#pragma once

#include <string>

namespace gamebase {

struct Config {
    Config();

    std::string shadersPath;
};

void loadConfig(const std::string& fileName);
const Config& config();

}
