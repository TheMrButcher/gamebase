/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/math/Math.h>
#include <algorithm>
#include <cmath>

namespace gamebase {

struct Color {
    Color(): r(0), g(0), b(0), a(1) {}
    Color(float r, float g, float b, float a = 1) : r(r), g(g), b(b), a(a) {}

    Color& operator*=(float num);

    float r;
    float g;
    float b;
    float a;
};

inline Color intColor(int r, int g, int b, int a = 255);
inline bool operator<(const Color& c1, const Color& c2);
inline bool operator==(const Color& c1, const Color& c2);
inline bool operator!=(const Color& c1, const Color& c2);
inline Color operator*(const Color& color, float num);
inline Color operator*(float num, const Color& color);
inline Color lerp(Color v1, Color v2, float part);

/////////////// IMPLEMENTATION ///////////////////

inline Color& Color::operator*=(float num)
{
    r = std::min(r * num, 1.0f);
    g = std::min(g * num, 1.0f);
    b = std::min(b * num, 1.0f);
    return *this;
}

inline Color intColor(int r, int g, int b, int a)
{
    return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

inline bool operator<(const Color& c1, const Color& c2)
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

inline bool operator==(const Color& c1, const Color& c2)
{
    return std::abs(c1.r - c2.r) < 0.004
        && std::abs(c1.g - c2.g) < 0.004
        && std::abs(c1.b - c2.b) < 0.004
        && std::abs(c1.a - c2.a) < 0.004;
}

inline bool operator!=(const Color& c1, const Color& c2)
{
    return !(c1 == c2);
}

inline Color operator*(const Color& color, float num)
{
    Color result = color;
    result *= num;
    return result;
}

inline Color operator*(float num, const Color& color)
{
    return color * num;
}

inline Color lerp(Color v1, Color v2, float part)
{
    return Color(
        lerp(v1.r, v2.r, part), lerp(v1.g, v2.g, part),
        lerp(v1.b, v2.b, part), lerp(v1.a, v2.a, part));
}

}
