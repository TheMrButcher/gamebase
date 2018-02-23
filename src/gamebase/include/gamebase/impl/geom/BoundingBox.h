/**
* Copyright (c) 2017 Slavnejshev Filipp
* This file is licensed under the terms of the MIT license.
*/

#pragma once

#include <gamebase/math/Vector2.h>
#include <gamebase/math/Transform2.h>
#include <iostream>

namespace gamebase { namespace impl {

struct BoundingBox {
    BoundingBox();
    BoundingBox(const Vec2& v);
    BoundingBox(const Vec2& v1, const Vec2& v2);
    BoundingBox(float width, float height, const Vec2& c = Vec2(0, 0));

    float left() const;
    float right() const;
    float bottom() const;
    float top() const;

    bool isValid() const;
    float width() const;
    float height() const;
    Vec2 size() const;
    float area() const;
    Vec2 center() const;

    bool contains(const Vec2& v) const;
    bool intersects(const BoundingBox& other) const;

    void add(const Vec2& v);
    void add(const BoundingBox& other);
    void move(const Vec2& v);
    void transform(const Transform2& tr);

    Vec2 bottomLeft;
    Vec2 topRight;
};

inline BoundingBox unite(const BoundingBox& b1, const BoundingBox& b2);
inline BoundingBox intersect(const BoundingBox& b1, const BoundingBox& b2);
inline BoundingBox extend(const BoundingBox& b, float ext);

inline bool operator==(const BoundingBox& b1, const BoundingBox& b2);
inline bool operator!=(const BoundingBox& b1, const BoundingBox& b2);
inline std::ostream& operator<<(std::ostream& stream, const BoundingBox& box);

/////////////// IMPLEMENTATION ///////////////////

inline BoundingBox::BoundingBox()
    : bottomLeft(
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max())
    , topRight(
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::lowest())
{}

inline BoundingBox::BoundingBox(const Vec2& v)
    : bottomLeft(v)
    , topRight(v)
{}

inline BoundingBox::BoundingBox(const Vec2& v1, const Vec2& v2)
    : bottomLeft(minVec(v1, v2))
    , topRight(maxVec(v1, v2))
{}

inline BoundingBox::BoundingBox(float width, float height, const Vec2& c)
    : bottomLeft(c.x - 0.5f * width, c.y - 0.5f * height)
    , topRight(c.x + 0.5f * width, c.y + 0.5f * height)
{}

inline float BoundingBox::left() const { return bottomLeft.x; }
inline float BoundingBox::right() const { return topRight.x; }
inline float BoundingBox::bottom() const { return bottomLeft.y; }
inline float BoundingBox::top() const { return topRight.y; }

inline bool BoundingBox::isValid() const
{
    return bottomLeft.x <= topRight.x && bottomLeft.y <= topRight.y;
}

inline float BoundingBox::width() const { return topRight.x - bottomLeft.x; }
inline float BoundingBox::height() const { return topRight.y - bottomLeft.y; }
inline Vec2 BoundingBox::size() const { return Vec2(width(), height()); }
inline float BoundingBox::area() const { return width() * height(); }

inline Vec2 BoundingBox::center() const
{
    return 0.5f * (bottomLeft + topRight);
}

inline bool BoundingBox::contains(const Vec2& v) const
{
    return bottomLeft.x <= v.x
        && bottomLeft.y <= v.y
        && v.x <= topRight.x
        && v.y <= topRight.y;
}

inline bool BoundingBox::intersects(const BoundingBox& other) const
{
    return bottomLeft.x <= other.topRight.x
        && bottomLeft.y <= other.topRight.y
        && topRight.x >= other.bottomLeft.x
        && topRight.y >= other.bottomLeft.y;
}

inline void BoundingBox::add(const Vec2& v)
{
    bottomLeft = minVec(bottomLeft, v);
    topRight = maxVec(topRight, v);
}

inline void BoundingBox::add(const BoundingBox& other)
{
    bottomLeft = minVec(bottomLeft, other.bottomLeft);
    topRight = maxVec(topRight, other.topRight);
}

inline void BoundingBox::move(const Vec2& v)
{
    bottomLeft += v;
    topRight += v;
}

inline void BoundingBox::transform(const Transform2& tr)
{
    if (tr.matrix.isIdentityMatrix()) {
        move(tr.offset);
        return;
    }

    Vec2 points[] = {
        bottomLeft, Vec2(bottomLeft.x, topRight.y),
        topRight, Vec2(topRight.x, bottomLeft.y) };

    BoundingBox result;
    for (size_t i = 0; i < 4; ++i)
        result.add(tr * points[i]);
    *this = result;
}

inline BoundingBox unite(const BoundingBox& b1, const BoundingBox& b2)
{
    BoundingBox result;
    result.bottomLeft = minVec(b1.bottomLeft, b2.bottomLeft);
    result.topRight = maxVec(b1.topRight, b2.topRight);
    return result;
}

inline BoundingBox intersect(const BoundingBox& b1, const BoundingBox& b2)
{
    BoundingBox result;
    result.bottomLeft = maxVec(b1.bottomLeft, b2.bottomLeft);
    result.topRight = minVec(b1.topRight, b2.topRight);
    return result;
}

inline BoundingBox extend(const BoundingBox& b, float ext)
{
    BoundingBox result;
    result.bottomLeft = b.bottomLeft - Vec2(ext, ext);
    result.topRight = b.topRight + Vec2(ext, ext);
    return result;
}

inline bool operator==(const BoundingBox& b1, const BoundingBox& b2)
{
    return b1.bottomLeft == b2.bottomLeft && b1.topRight == b2.topRight;
}

inline bool operator!=(const BoundingBox& b1, const BoundingBox& b2)
{
    return !(b1 == b2);
}

inline std::ostream& operator<<(std::ostream& stream, const BoundingBox& box)
{
    stream << box.bottomLeft << " x " << box.topRight;
    return stream;
}

} }
