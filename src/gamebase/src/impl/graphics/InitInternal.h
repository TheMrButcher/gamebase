/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

namespace gamebase { namespace impl {
static const char* DEFAULT_CONFIG_NAME = "config.json";

void initWindowModeInternal(int* argc, char** argv,
    int width, int height, const std::string& name,
    int posX, int posY);

void initGameModeInternal(int* argc, char** argv, int width, int height);
} }
