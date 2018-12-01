/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/Window.h>

namespace gamebase { namespace impl {
static const char* DEFAULT_CONFIG_NAME = "Config.json";

std::unique_ptr<sf::RenderWindow> initWindowImpl(
    int* argc, char** argv, int width, int height,
    const std::string& title, GraphicsMode::Enum mode, bool maximize);
} }
