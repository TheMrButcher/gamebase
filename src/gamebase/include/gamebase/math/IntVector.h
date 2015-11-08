#pragma once

#include <gamebase/utils/Exception.h>
#include <algorithm>
#include <ostream>

namespace gamebase {

struct IntVec2 {
    IntVec2(): x(0), y(0) {}
    IntVec2(int x, int y): x(x), y(y) {}

    float length() const { return std::sqrtf(static_cast<float>(x * x + y * y)); }

    bool isZero() const { return x == 0 && y == 0; }

    IntVec2 operator-() const
    {
        return IntVec2(-x, -y);
    }

    IntVec2& operator+=(const IntVec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    IntVec2& operator-=(const IntVec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    IntVec2& operator*=(int num)
    {
        x *= num;
        y *= num;
        return *this;
    }

    int x;
    int y;
};

inline IntVec2 operator+(const IntVec2& v1, const IntVec2& v2)
{
    return IntVec2(v1.x + v2.x, v1.y + v2.y);
}

inline IntVec2 operator-(const IntVec2& v1, const IntVec2& v2)
{
    return IntVec2(v1.x - v2.x, v1.y - v2.y);
}

inline IntVec2 operator*(const IntVec2& v, int num)
{
    return IntVec2(v.x * num, v.y * num);
}

inline int dot(const IntVec2& v1, const IntVec2& v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

inline int cross(const IntVec2& v1, const IntVec2& v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

inline IntVec2 rotate90(const IntVec2& v)
{
    return IntVec2(-v.y, v.x);
}

inline IntVec2 rotate90CCW(const IntVec2& v)
{
    return rotate90(v);
}

inline IntVec2 rotate90CW(const IntVec2& v)
{
    return IntVec2(v.y, -v.x);
}

inline IntVec2 minVec(const IntVec2& v1, const IntVec2& v2)
{
    return IntVec2(std::min(v1.x, v2.x), std::min(v1.y, v2.y));
}

inline IntVec2 maxVec(const IntVec2& v1, const IntVec2& v2)
{
    return IntVec2(std::max(v1.x, v2.x), std::max(v1.y, v2.y));
}

inline std::ostream& operator<<(std::ostream& stream, const IntVec2& v)
{
    stream << "(" << v.x << ", " << v.y << ")";
    return stream;
}

}
