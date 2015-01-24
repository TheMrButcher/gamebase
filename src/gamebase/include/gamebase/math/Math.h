#pragma once

#include <cmath>

namespace gamebase {

template <typename T>
T clamp(T value, T minValue, T maxValue)
{
    return std::min(std::max(value, minValue), maxValue);
}

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
