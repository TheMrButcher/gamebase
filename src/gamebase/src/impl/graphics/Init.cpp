/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "InitInternal.h"
#include "State.h"
#include "src/impl/global/Config.h"
#include "src/impl/global/GlobalResources.h"
#include <gamebase/impl/app/Config.h>
#include <gamebase/tools/Exception.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <Magick++/Functions.h>
#include <iostream>

namespace gamebase { namespace impl {
namespace {
void initImageMagick(int* /* argc */, char** argv)
{
    static bool isInited = false;
    if (isInited)
        return;
    Magick::InitializeMagick(argv[0]);
    isInited = true;
}

void initGlew()
{
    static bool isInited = false;
    if (isInited)
        return;
    std::cout << "Initing Glew... ";
    GLenum result = glewInit();
    if (result != GLEW_OK) {
        std::ostringstream ss;
        ss << "Error while loading Glew: " << glewGetErrorString(result);
        THROW_EX() << ss.str();
    }
    std::cout << "Done" << std::endl;
    isInited = true;
}

std::unique_ptr<sf::RenderWindow> initWindowImpl(
    int* argc, char** argv, const sf::VideoMode& videoMode,
    const sf::String& title, sf::Uint32 style)
{
    initImageMagick(argc, argv);

    sf::ContextSettings contextSettings;
    auto windowImpl = std::make_unique<sf::RenderWindow>(
        videoMode, title, style, contextSettings);
    windowImpl->setVerticalSyncEnabled(true);
    windowImpl->setActive(true);
    initGlew();

    initState(videoMode.width, videoMode.height);
    loadGlobalResources();
    return windowImpl;
}
}

std::unique_ptr<sf::RenderWindow> initWindowImpl(
     int* argc, char** argv, int width, int height,
    const std::string& titleUtf8, GraphicsMode::Enum mode)
{
    sf::VideoMode videoMode(width, height, 32);
    sf::Uint32 style = sf::Style::Default;
    switch (mode) {
    case GraphicsMode::Fullscreen: style = sf::Style::Fullscreen; break;
    case GraphicsMode::WindowNoResize: style = sf::Style::Titlebar | sf::Style::Close; break;
    default: style = sf::Style::Default;; break;
    }
    auto title = sf::String::fromUtf8(titleUtf8.begin(), titleUtf8.end());
    return initWindowImpl(argc, argv, videoMode, title, style);
}

} }
