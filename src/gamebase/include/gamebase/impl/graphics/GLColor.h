/**
* Copyright (c) 2018 Slavnejshev Filipp
* This file is licensed under the terms of the MIT license.
*/

#pragma once

#include <gamebase/graphics/Color.h>
#include <gamebase/math/Math.h>
#include <algorithm>
#include <cmath>

namespace gamebase { namespace impl {

struct GLColor {
    GLColor() : r(0), g(0), b(0), a(1) {}
    GLColor(float r, float g, float b, float a = 1) : r(r), g(g), b(b), a(a) {}

    Color intColor() const;
    GLColor& operator*=(float num);

    float r;
    float g;
    float b;
    float a;
};

inline GLColor makeGLColor(int r, int g, int b, int a);
inline GLColor makeGLColor(const Color& color);
inline bool operator<(const GLColor& c1, const GLColor& c2);
inline bool operator==(const GLColor& c1, const GLColor& c2);
inline bool operator!=(const GLColor& c1, const GLColor& c2);
inline GLColor operator*(const GLColor& color, float num);
inline GLColor operator*(float num, const GLColor& color);

/////////////// IMPLEMENTATION ///////////////////

inline GLColor& GLColor::operator*=(float num)
{
    r = r * num;
    g = g * num;
    b = b * num;
    return *this;
}

inline Color GLColor::intColor() const
{
    return Color(
        static_cast<int>(r * 255.0f + 0.5f),
        static_cast<int>(g * 255.0f + 0.5f),
        static_cast<int>(b * 255.0f + 0.5f),
        static_cast<int>(a * 255.0f + 0.5f));
}

inline GLColor makeGLColor(int r, int g, int b, int a)
{
    return GLColor(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

inline GLColor makeGLColor(const Color& color)
{
    return makeGLColor(color.r, color.g, color.b, color.a);
}

inline bool operator<(const GLColor& c1, const GLColor& c2)
{
    if (std::abs(c1.r - c2.r) > 0.004)
        return c1.r < c2.r;

    if (std::abs(c1.g - c2.g) > 0.004)
        return c1.g < c2.g;

    if (std::abs(c1.b - c2.b) > 0.004)
        return c1.b < c2.b;

    if (std::abs(c1.a - c2.a) > 0.004)
        return c1.a < c2.a;

    return false;
}

inline bool operator==(const GLColor& c1, const GLColor& c2)
{
    return std::abs(c1.r - c2.r) < 0.004
        && std::abs(c1.g - c2.g) < 0.004
        && std::abs(c1.b - c2.b) < 0.004
        && std::abs(c1.a - c2.a) < 0.004;
}

inline bool operator!=(const GLColor& c1, const GLColor& c2)
{
    return !(c1 == c2);
}

inline GLColor operator*(const GLColor& color, float num)
{
    GLColor result = color;
    result *= num;
    return result;
}

inline GLColor operator*(float num, const GLColor& color)
{
    return color * num;
}

}

inline impl::GLColor lerp(impl::GLColor v1, impl::GLColor v2, float part)
{
    return impl::GLColor(
        lerp(v1.r, v2.r, part), lerp(v1.g, v2.g, part),
        lerp(v1.b, v2.b, part), lerp(v1.a, v2.a, part));
}

}
