/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/tools/Exception.h>
#include <algorithm>
#include <ostream>

namespace gamebase {

struct IntVec2;
typedef IntVec2 IVec2;

struct IntVec2 {
    IntVec2(): x(0), y(0) {}
    IntVec2(int x, int y): x(x), y(y) {}

    float length() const;
    bool isZero() const;
    IntVec2 operator-() const;
    IntVec2& operator+=(const IntVec2& other);
    IntVec2& operator-=(const IntVec2& other);
    IntVec2& operator*=(int num);

    int x;
    int y;
};

inline bool operator==(const IntVec2& v1, const IntVec2& v2);
inline bool operator!=(const IntVec2& v1, const IntVec2& v2);
inline bool operator<(const IntVec2& v1, const IntVec2& v2);
inline bool operator>(const IntVec2& v1, const IntVec2& v2);
inline IntVec2 operator+(const IntVec2& v1, const IntVec2& v2);
inline IntVec2 operator-(const IntVec2& v1, const IntVec2& v2);
inline float dist(const IntVec2& v1, const IntVec2& v2);
inline IntVec2 operator*(const IntVec2& v, int num);
inline IntVec2 operator*(int num, const IntVec2& v);
inline int dot(const IntVec2& v1, const IntVec2& v2);
inline int cross(const IntVec2& v1, const IntVec2& v2);
inline IntVec2 rotate90(const IntVec2& v);
inline IntVec2 rotate90CCW(const IntVec2& v);
inline IntVec2 rotate90CW(const IntVec2& v);
inline IntVec2 minVec(const IntVec2& v1, const IntVec2& v2);
inline IntVec2 maxVec(const IntVec2& v1, const IntVec2& v2);
inline std::ostream& operator<<(std::ostream& stream, const IntVec2& v);

/////////////// IMPLEMENTATION ///////////////////

inline float IntVec2::length() const { return std::sqrtf(static_cast<float>(x * x + y * y)); }

inline bool IntVec2::isZero() const { return x == 0 && y == 0; }

inline IntVec2 IntVec2::operator-() const
{
    return IntVec2(-x, -y);
}

inline IntVec2& IntVec2::operator+=(const IntVec2& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

inline IntVec2& IntVec2::operator-=(const IntVec2& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

inline IntVec2& IntVec2::operator*=(int num)
{
    x *= num;
    y *= num;
    return *this;
}

inline bool operator==(const IntVec2& v1, const IntVec2& v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

inline bool operator!=(const IntVec2& v1, const IntVec2& v2)
{
    return !(v1 == v2);
}

inline bool operator<(const IntVec2& v1, const IntVec2& v2)
{
    if (v1.x != v2.x)
        return v1.x < v2.x;
    return v1.y < v2.y;
}

inline bool operator>(const IntVec2& v1, const IntVec2& v2)
{
    return v2 < v1;
}

inline IntVec2 operator+(const IntVec2& v1, const IntVec2& v2)
{
    return IntVec2(v1.x + v2.x, v1.y + v2.y);
}

inline IntVec2 operator-(const IntVec2& v1, const IntVec2& v2)
{
    return IntVec2(v1.x - v2.x, v1.y - v2.y);
}

inline float dist(const IntVec2& v1, const IntVec2& v2)
{
    return (v1 - v2).length();
}

inline IntVec2 operator*(const IntVec2& v, int num)
{
    return IntVec2(v.x * num, v.y * num);
}

inline IntVec2 operator*(int num, const IntVec2& v)
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
