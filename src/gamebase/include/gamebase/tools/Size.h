/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/math/Vector2.h>

namespace gamebase {

struct Size {
    Size() : width(0), height(0) {}
    Size(unsigned int w, unsigned int h) : width(w), height(h) {}

    Vec2 toVector() const
    {
        return Vec2(static_cast<float>(width), static_cast<float>(height));
    }

    unsigned int width;
    unsigned int height;
};

}
