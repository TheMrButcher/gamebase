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

void setNewPath(std::string& dst, const std::string& path)
{
    if (path.empty())
        return;
    dst = addSlash(path);
}
}

Config::Config()
    : shadersPath("resources\\shaders\\")
    , imagesPath("resources\\images\\")
    , fontsPath("resources\\fonts\\")
{}

void loadConfig(const std::string& fileName)
{
    std::cout << "Loading config... ";
    Config newConfig = globalConfig;
    try {
        std::string configStr = loadTextFile(fileName);
        Json::Reader reader;
        Json::Value rootValue;
        reader.parse(configStr, rootValue, false);

        setNewPath(newConfig.shadersPath, rootValue["shadersPath"].asString());
        setNewPath(newConfig.imagesPath, rootValue["imagesPath"].asString());
        setNewPath(newConfig.fontsPath, rootValue["fontsPath"].asString());
    } catch (std::exception& ex) {
        std::cerr << "Failed to load config from " << fileName << ". "
            << "Reason: " << ex.what() << ". Using default config" << std::endl;
        return;
    }
    globalConfig = newConfig;
    std::cout << "Done" << std::endl;

    std::cout << "Path to shaders: " << globalConfig.shadersPath << std::endl;
    std::cout << "Path to images: " << globalConfig.imagesPath << std::endl;
    std::cout << "Path to fonts: " << globalConfig.fontsPath << std::endl;
}

const Config& config()
{
    return globalConfig;
}

}
