#pragma once

#include <cmath>

namespace gamebase {

inline unsigned int uround(float f)
{
    return static_cast<unsigned int>(f + 0.5f);
}

inline int round(float f)
{
    return static_cast<int>(f + 0.5f);
}

const float EPSILON = 0.00001f;

inline bool isEqual(float a, float b, float eps = EPSILON)
{
    return std::abs(a - b) < eps;
}

}
