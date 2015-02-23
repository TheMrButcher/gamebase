#pragma once

#include <gamebase/math/Matrix2.h>
#include <gamebase/utils/Exception.h>
#include <algorithm>
#include <ostream>

namespace gamebase {

struct Vec2 {
    Vec2(): x(0), y(0) {}
    Vec2(float x, float y): x(x), y(y) {}

    float length() const { return std::sqrtf(x * x + y * y); }

    bool isZero() const { return x == 0 && y == 0; }

    Vec2 operator-() const
    {
        return Vec2(-x, -y);
    }

    Vec2& operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2& operator*=(float num)
    {
        x *= num;
        y *= num;
        return *this;
    }

    Vec2& normalize()
    {
        if (isZero())
            THROW_EX() << "Can't normalize zero-sized vector";
        return operator*=(1.0f / length());
    }

    float x;
    float y;
};

inline Vec2 operator+(const Vec2& v1, const Vec2& v2)
{
    return Vec2(v1.x + v2.x, v1.y + v2.y);
}

inline Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
    return Vec2(v1.x - v2.x, v1.y - v2.y);
}

inline Vec2 operator*(const Vec2& v, float num)
{
    return Vec2(v.x * num, v.y * num);
}

inline float dot(const Vec2& v1, const Vec2& v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

inline float cross(const Vec2& v1, const Vec2& v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

inline Vec2 rotate90(const Vec2& v)
{
    return Vec2(-v.y, v.x);
}

inline Vec2 minVec(const Vec2& v1, const Vec2& v2)
{
    return Vec2(std::min(v1.x, v2.x), std::min(v1.y, v2.y));
}

inline Vec2 maxVec(const Vec2& v1, const Vec2& v2)
{
    return Vec2(std::max(v1.x, v2.x), std::max(v1.y, v2.y));
}

inline Vec2 operator*(const Matrix2& m, const Vec2& v)
{
    return Vec2(
        m.get(0, 0) * v.x + m.get(0, 1) * v.y,
        m.get(1, 0) * v.x + m.get(1, 1) * v.y);
}

inline std::ostream& operator<<(std::ostream& stream, const Vec2& v)
{
    stream << "(" << v.x << ", " << v.y << ")";
    return stream;
}

}
