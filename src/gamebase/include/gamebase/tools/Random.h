/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <cstdlib>

namespace gamebase {

GAMEBASE_API void randomize();

inline int randomInt(int minVal, int maxVal)
{
    return minVal + rand() % (maxVal - minVal + 1);
}

inline float randomFloat()
{
    return static_cast<float>(rand()) / RAND_MAX;
}

inline double randomDouble()
{
    return static_cast<double>(rand()) / RAND_MAX;
}

}
