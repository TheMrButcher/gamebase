#include <stdafx.h>
#include "Config.h"
#include <gamebase/core/Core.h>
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

void setPath(const Json::Value& src, const std::string& memberName, std::string& dst)
{
    if (!src.isMember(memberName))
        return;
    auto path = src[memberName].asString();
    if (path.empty())
        return;
    dst = addSlash(path);
}

void loadConfigFromString(const std::string& configStr, bool printStats)
{
}

void loadConfigFromFile(const std::string& fileName)
{
    std::cout << "Loading config... ";
    Config newConfig = globalConfig;
    try {
        std::string configStr = loadTextFile(fileName);
        Json::Reader reader;
        Json::Value rootValue;
        reader.parse(configStr, rootValue, false);

        if (rootValue.isMember("windowName"))
            newConfig.windowName = rootValue["windowName"].asString();
        if (rootValue.isMember("width"))
            newConfig.width = rootValue["width"].asInt();
        if (rootValue.isMember("height"))
            newConfig.height = rootValue["height"].asInt();
        if (rootValue.isMember("mode")) {
            std::string modeStr = rootValue["mode"].asString();
            if (modeStr == "Window" || modeStr == "window" || modeStr == "Windowed" || modeStr == "windowed")
                newConfig.mode = GraphicsMode::Window;
            else if (modeStr == "Fullscreen" || modeStr == "fullscreen" || modeStr == "Full" || modeStr == "full")
                newConfig.mode = GraphicsMode::Fullscreen;
            else
                std::cerr << "Wrong value of mode: " << modeStr << ". Skipping" << std::endl;
        }

        setPath(rootValue, "shadersPath", newConfig.shadersPath);
        setPath(rootValue, "imagesPath", newConfig.imagesPath);
        setPath(rootValue, "fontsPath", newConfig.fontsPath);
        setPath(rootValue, "designPath", newConfig.designPath);
    } catch (std::exception& ex) {
        std::cerr << "Failed to load config from " << fileName << ". "
            << "Reason: " << ex.what() << ". Using default config" << std::endl;
        return;
    }
    globalConfig = newConfig;
    std::cout << "Done" << std::endl;

    std::cout << "Width (from config): " << globalConfig.width << std::endl;
    std::cout << "Height (from config): " << globalConfig.height << std::endl;
    std::cout << "Mode (from config): " << (globalConfig.mode == GraphicsMode::Window
            ? std::string("Window") : std::string("Fullscreen"))
        << std::endl;

    std::cout << "Path to shaders: " << globalConfig.shadersPath << std::endl;
    std::cout << "Path to images: " << globalConfig.imagesPath << std::endl;
    std::cout << "Path to fonts: " << globalConfig.fontsPath << std::endl;
    std::cout << "Path to design: " << globalConfig.designPath << std::endl;
}
}

Config::Config()
    : shadersPath("resources\\shaders\\")
    , imagesPath("resources\\images\\")
    , fontsPath("resources\\fonts\\")
    , designPath("resource\\design\\")
    , mode(GraphicsMode::Window)
    , windowName("Gamebased Application")
    , width(800)
    , height(600)
{}

void configurateFromString(const std::string& configStr, bool printStats)
{
    std::cout << "Parsing config... ";
    Config newConfig = globalConfig;
    try {
        Json::Reader reader;
        Json::Value rootValue;
        reader.parse(configStr, rootValue, false);

        if (rootValue.isMember("windowName"))
            newConfig.windowName = rootValue["windowName"].asString();
        if (rootValue.isMember("width"))
            newConfig.width = rootValue["width"].asInt();
        if (rootValue.isMember("height"))
            newConfig.height = rootValue["height"].asInt();
        if (rootValue.isMember("mode")) {
            std::string modeStr = rootValue["mode"].asString();
            if (modeStr == "Window" || modeStr == "window" || modeStr == "Windowed" || modeStr == "windowed")
                newConfig.mode = GraphicsMode::Window;
            else if (modeStr == "Fullscreen" || modeStr == "fullscreen" || modeStr == "Full" || modeStr == "full")
                newConfig.mode = GraphicsMode::Fullscreen;
            else
                std::cerr << "Wrong value of mode: " << modeStr << ". Skipping" << std::endl;
        }

        setPath(rootValue, "shadersPath", newConfig.shadersPath);
        setPath(rootValue, "imagesPath", newConfig.imagesPath);
        setPath(rootValue, "fontsPath", newConfig.fontsPath);
        setPath(rootValue, "designPath", newConfig.designPath);
    } catch (std::exception& ex) {
        std::cerr << "Failed to parse config. "
            << "Reason: " << ex.what() << ". Using default config" << std::endl;
        return;
    }
    globalConfig = newConfig;
    std::cout << "Done" << std::endl;

    if (printStats) {
        std::cout << "Width (from config): " << globalConfig.width << std::endl;
        std::cout << "Height (from config): " << globalConfig.height << std::endl;
        std::cout << "Mode (from config): " << (globalConfig.mode == GraphicsMode::Window
                ? std::string("Window") : std::string("Fullscreen"))
            << std::endl;

        std::cout << "Path to shaders: " << globalConfig.shadersPath << std::endl;
        std::cout << "Path to images: " << globalConfig.imagesPath << std::endl;
        std::cout << "Path to fonts: " << globalConfig.fontsPath << std::endl;
        std::cout << "Path to design: " << globalConfig.designPath << std::endl;
    }
}

void configurateFromFile(const std::string& fileName, bool printStats)
{
    try {
        std::cout << "Loading config... ";
        std::string configStr = loadTextFile(fileName);
        configurateFromString(configStr, printStats);
    } catch (std::exception& ex) {
        std::cerr << "Failed to load config from " << fileName << ". "
            << "Reason: " << ex.what() << std::endl;
        return;
    }
}

const Config& config()
{
    return globalConfig;
}
}
