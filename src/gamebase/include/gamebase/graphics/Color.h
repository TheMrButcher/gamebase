#pragma once

#include <gamebase/math/Math.h>
#include <algorithm>

namespace gamebase {

struct Color {
    Color(): r(0), g(0), b(0), a(1) {}
    Color(float r, float g, float b, float a = 1)
        : r(r), g(g), b(b), a(a)
    {}

    Color& operator*=(float num)
    {
        r = std::min(r * num, 1.0f);
        g = std::min(g * num, 1.0f);
        b = std::min(b * num, 1.0f);
        return *this;
    }

    float r;
    float g;
    float b;
    float a;
};

inline Color operator*(const Color& color, float num)
{
    Color result = color;
    result *= num;
    return result;
}

inline Color lerp(Color v1, Color v2, float part)
{
    return Color(
        lerp(v1.r, v2.r, part), lerp(v1.g, v2.g, part),
        lerp(v1.b, v2.b, part), lerp(v1.a, v2.a, part));
}

}
