/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "Config.h"
#include <gamebase/tools/FileIO.h>
#include <gamebase/tools/Exception.h>
#include <gamebase/text/StringUtils.h>
#include <json/reader.h>
#include <iostream>

namespace gamebase { namespace impl {
namespace {
Config globalConfig;

enum class ConfigVersion {
    VER2,
    VER3
};

const char* toString(ConfigVersion version)
{
    switch (version) {
    case ConfigVersion::VER3: return "VER3";
    default: THROW_EX() << "Unknown config version: " << static_cast<int>(version);
    }
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
}

Config::Config()
    : shadersPath("resources\\shaders\\")
    , imagesPath("resources\\images\\")
    , soundsPath("resources\\sounds\\")
    , musicPath("resources\\music\\")
    , fontsPath("resources\\fonts\\")
    , designPath("resources\\design\\")
    , mode(GraphicsMode::Window)
    , windowTitle("Gamebased Application")
    , width(800)
    , height(600)
{}

void configurateFromString(const std::string& configStr, bool printStats)
{
    if (printStats)
        std::cout << "Parsing config... ";
    Config newConfig = globalConfig;
    try {
        Json::Reader reader;
        Json::Value rootValue;
        reader.parse(configStr, rootValue, false);

        ConfigVersion version = ConfigVersion::VER2;
        if (rootValue.isMember("version")) {
            auto versionStr = rootValue["version"].asString();
            if (versionStr == toString(ConfigVersion::VER3)) {
                version = ConfigVersion::VER3;
            } else {
                THROW_EX() << "Unknown config version: " << versionStr;
            }
        }
        if (rootValue.isMember("windowTitle"))
            newConfig.windowTitle = rootValue["windowTitle"].asString();
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
        if (version >= ConfigVersion::VER3) {
            setPath(rootValue, "soundsPath", newConfig.soundsPath);
            setPath(rootValue, "musicPath", newConfig.musicPath);
        } else {
            newConfig.soundsPath = newConfig.imagesPath + "..\\sounds";
            newConfig.musicPath = newConfig.imagesPath + "..\\music";
        }
        setPath(rootValue, "fontsPath", newConfig.fontsPath);
        setPath(rootValue, "designPath", newConfig.designPath);

        auto memberNames = rootValue.getMemberNames();
        for (auto it = memberNames.begin(); it != memberNames.end(); ++it)
            newConfig.dict[*it] = rootValue[*it].asString();

        newConfig.configSource = configStr;
    } catch (std::exception& ex) {
        std::cerr << "Failed to parse config. "
            << "Reason: " << ex.what() << ". Using default config" << std::endl;
        return;
    }
    globalConfig = newConfig;

    if (printStats) {
        std::cout << "Done" << std::endl;
        std::cout << "Width (from config): " << globalConfig.width << std::endl;
        std::cout << "Height (from config): " << globalConfig.height << std::endl;
        std::cout << "Mode (from config): " << (globalConfig.mode == GraphicsMode::Window
                ? std::string("Window") : std::string("Fullscreen"))
            << std::endl;

        std::cout << "Path to shaders: " << globalConfig.shadersPath << std::endl;
        std::cout << "Path to images: " << globalConfig.imagesPath << std::endl;
        std::cout << "Path to sounds: " << globalConfig.soundsPath << std::endl;
        std::cout << "Path to music: " << globalConfig.musicPath << std::endl;
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

const std::string& configAsString()
{
    return config().configSource;
}

const Dictionary& configAsDictionary()
{
    return config().dict;
}

const Config& config()
{
    return globalConfig;
}

} }
