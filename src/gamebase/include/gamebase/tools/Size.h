/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/math/Vector2.h>
#include <ostream>

namespace gamebase {

struct Size {
    Size() : w(0), h(0) {}
    Size(unsigned int w, unsigned int h) : w(w), h(h) {}

    Vec2 toVector() const
    {
        return Vec2(static_cast<float>(w), static_cast<float>(h));
    }

    unsigned int w;
    unsigned int h;
};

inline bool operator==(const Size& s1, const Size& s2)
{
    return s1.w == s2.w && s1.h == s2.h;
}

inline bool operator!=(const Size& s1, const Size& s2)
{
    return !(s1 == s2);
}

inline std::ostream& operator<<(std::ostream& stream, const Size& size)
{
    return stream << size.w << " x " << size.h;
}

}
