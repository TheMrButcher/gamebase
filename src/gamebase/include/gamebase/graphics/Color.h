/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/math/Math.h>
#include <algorithm>
#include <cmath>

namespace gamebase {

struct Color {
    Color(): r(0), g(0), b(0), a(255) {}
    Color(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}

    Color& operator*=(float num);

    int r;
    int g;
    int b;
    int a;
};

inline bool operator<(const Color& c1, const Color& c2);
inline bool operator==(const Color& c1, const Color& c2);
inline bool operator!=(const Color& c1, const Color& c2);
inline Color operator*(const Color& color, float num);
inline Color operator*(float num, const Color& color);
inline Color lerp(Color v1, Color v2, float part);

/////////////// IMPLEMENTATION ///////////////////

inline Color& Color::operator*=(float num)
{
    r = static_cast<int>(r * num + 0.5f);
    g = static_cast<int>(g * num + 0.5f);
    b = static_cast<int>(b * num + 0.5f);
    return *this;
}

inline bool operator<(const Color& c1, const Color& c2)
{
    if (c1.r != c2.r)
        return c1.r < c2.r;

    if (c1.g != c2.g)
        return c1.g < c2.g;

    if (c1.b != c2.b)
        return c1.b < c2.b;

    if (c1.a != c2.a)
        return c1.a < c2.a;

    return false;
}

inline bool operator==(const Color& c1, const Color& c2)
{
    return c1.r == c2.r
        && c1.g == c2.g
        && c1.b == c2.b
        && c1.a == c2.a;
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
