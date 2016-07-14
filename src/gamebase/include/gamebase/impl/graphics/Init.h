/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <string>

namespace gamebase { namespace impl {

void GAMEBASE_API initWindowMode(int* argc, char** argv,
    int width, int height, const std::string& name,
    int posX, int posY);

inline void initWindowMode(int* argc, char** argv,  
    int width, int height, const std::string& name)
{
    initWindowMode(argc, argv, width, height, name, 0, 0);
}

inline void initWindowMode(int* argc, char** argv, int width, int height)
{
    initWindowMode(argc, argv, width, height, "Game", 0, 0);
}

void GAMEBASE_API initGameMode(int* argc, char** argv, int width, int height);

} }
