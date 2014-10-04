#pragma once

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

}
