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
#include <SFML/Window/Event.hpp>

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
    int* argc, char** argv, sf::VideoMode videoMode,
    const sf::String& title, sf::Uint32 style, bool maximize)
{
    initImageMagick(argc, argv);

    sf::ContextSettings contextSettings;
    auto windowImpl = std::make_unique<sf::RenderWindow>(
        videoMode, title, style, contextSettings);
    windowImpl->setVerticalSyncEnabled(true);
    windowImpl->setActive(true);
    initGlew();

	if (maximize) {
		ShowWindow(windowImpl->getSystemHandle(), SW_MAXIMIZE);
		RECT rect;
		GetClientRect(windowImpl->getSystemHandle(), &rect);
		std::cout << "After maximize: " << rect.right - rect.left << ", " << rect.bottom - rect.top << std::endl;
		videoMode.width = rect.right - rect.left;
		videoMode.height = rect.bottom - rect.top;
		windowImpl->setSize(sf::Vector2u(videoMode.width, videoMode.height));
		glViewport(0, 0, static_cast<GLsizei>(videoMode.width), static_cast<GLsizei>(videoMode.height));
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		windowImpl->display();
	}

    initState(videoMode.width, videoMode.height);
    loadGlobalResources();
    return windowImpl;
}
}

std::unique_ptr<sf::RenderWindow> initWindowImpl(
     int* argc, char** argv, int width, int height,
    const std::string& titleUtf8, GraphicsMode::Enum mode, bool maximize)
{
    sf::VideoMode videoMode(width, height, 32);
    sf::Uint32 style = sf::Style::Default;
    switch (mode) {
    case GraphicsMode::Fullscreen:
		style = sf::Style::Fullscreen;
		maximize = false;
		break;
    case GraphicsMode::WindowNoResize:
		style = sf::Style::Titlebar | sf::Style::Close;
		maximize = false;
		break;
    default:
		style = sf::Style::Default;
		break;
    }
    auto title = sf::String::fromUtf8(titleUtf8.begin(), titleUtf8.end());
    return initWindowImpl(argc, argv, videoMode, title, style, maximize);
}

} }
