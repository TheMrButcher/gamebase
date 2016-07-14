/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/math/Matrix2.h>
#include <gamebase/tools/Exception.h>
#include <algorithm>
#include <ostream>

namespace gamebase {

struct Vec2;
typedef Vec2 Point;

struct GAMEBASE_API Vec2 {
    Vec2(): x(0), y(0) {}
    Vec2(float x, float y): x(x), y(y) {}

    float length() const;
    bool isZero() const;
    Vec2& normalize();

    float angle() const;
    void rotate(float angle);
    void setAngle(float newAngle);

    Vec2 operator-() const;
    Vec2& operator+=(const Vec2& other);
    Vec2& operator-=(const Vec2& other);
    Vec2& operator*=(float num);
    Vec2& operator/=(float num);

    float x;
    float y;
};

inline bool operator==(const Vec2& v1, const Vec2& v2);
inline bool operator!=(const Vec2& v1, const Vec2& v2);
inline bool operator<(const Vec2& v1, const Vec2& v2);
inline bool operator>(const Vec2& v1, const Vec2& v2);
inline Vec2 operator+(const Vec2& v1, const Vec2& v2);
inline Vec2 operator-(const Vec2& v1, const Vec2& v2);
inline float dist(const Vec2& v1, const Vec2& v2);
inline Vec2 operator*(const Vec2& v, float num);
inline Vec2 operator*(float num, const Vec2& v);
inline Vec2 operator/(const Vec2& v, float num);
inline float dot(const Vec2& v1, const Vec2& v2);
inline float cross(const Vec2& v1, const Vec2& v2);
inline Vec2 rotate90(const Vec2& v);
inline Vec2 minVec(const Vec2& v1, const Vec2& v2);
inline Vec2 maxVec(const Vec2& v1, const Vec2& v2);
inline Vec2 operator*(const Matrix2& m, const Vec2& v);
inline std::ostream& operator<<(std::ostream& stream, const Vec2& v);

/////////////// IMPLEMENTATION ///////////////////

inline float Vec2::length() const { return std::sqrtf(x * x + y * y); }

inline bool Vec2::isZero() const { return x == 0 && y == 0; }

inline Vec2& Vec2::normalize()
{
    if (isZero())
        THROW_EX() << "Can't normalize zero-sized vector";
    return operator*=(1.0f / length());
}

inline void Vec2::setAngle(float newAngle) { rotate(newAngle - angle()); }

inline Vec2 Vec2::operator-() const
{
    return Vec2(-x, -y);
}

inline Vec2& Vec2::operator+=(const Vec2& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

inline Vec2& Vec2::operator-=(const Vec2& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

inline Vec2& Vec2::operator*=(float num)
{
    x *= num;
    y *= num;
    return *this;
}

inline Vec2& Vec2::operator/=(float num)
{
    x /= num;
    y /= num;
    return *this;
}

inline bool operator==(const Vec2& v1, const Vec2& v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

inline bool operator!=(const Vec2& v1, const Vec2& v2)
{
    return !(v1 == v2);
}

inline bool operator<(const Vec2& v1, const Vec2& v2)
{
    if (v1.x != v2.x)
        return v1.x < v2.x;
    return v1.y < v2.y;
}

inline bool operator>(const Vec2& v1, const Vec2& v2)
{
    return v2 < v1;
}

inline Vec2 operator+(const Vec2& v1, const Vec2& v2)
{
    return Vec2(v1.x + v2.x, v1.y + v2.y);
}

inline Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
    return Vec2(v1.x - v2.x, v1.y - v2.y);
}

inline float dist(const Vec2& v1, const Vec2& v2)
{
    return (v1 - v2).length();
}

inline Vec2 operator*(const Vec2& v, float num)
{
    return Vec2(v.x * num, v.y * num);
}

inline Vec2 operator*(float num, const Vec2& v)
{
    return Vec2(v.x * num, v.y * num);
}

inline Vec2 operator/(const Vec2& v, float num)
{
    return Vec2(v.x / num, v.y / num);
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
