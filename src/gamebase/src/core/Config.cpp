#include <stdafx.h>
#include "Config.h"
#include <gamebase/utils/FileIO.h>
#include <json/reader.h>
#include <iostream>

namespace gamebase {
namespace {
Config globalConfig;

std::string addSlash(const std::string& path)
{
    if (path.empty() || path.back() == '\\')
        return path;
    return path + '\\';
}
}

Config::Config()
    : shadersPath("resources\\shaders\\")
{}

void loadConfig(const std::string& fileName)
{
    Config newConfig;
    try {
        std::string configStr = loadTextFile(fileName);
        Json::Reader reader;
        Json::Value rootValue;
        reader.parse(configStr, rootValue, false);

        newConfig.shadersPath = addSlash(rootValue["shadersPath"].asString());
        std::cout << "shadersPath: " << newConfig.shadersPath << std::endl;
    } catch (std::exception& ex) {
        std::cerr << "Failed to load config from " << fileName << ". "
            << "Reason: " << ex.what() << ". Using default config" << std::endl;
        return;
    }
    globalConfig = newConfig;
}

const Config& config()
{
    return globalConfig;
}

}
