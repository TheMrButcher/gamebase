#pragma once

#include <gamebase/math/Vector2.h>
#include <gamebase/math/Transform2.h>
#include <iostream>

namespace gamebase {

struct BoundingBox {
    BoundingBox()
        : bottomLeft(
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max())
        , topRight(
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest())
    {}

    BoundingBox(const Vec2& v)
        : bottomLeft(v)
        , topRight(v)
    {}

    BoundingBox(const Vec2& v1, const Vec2& v2)
        : bottomLeft(minVec(v1, v2))
        , topRight(maxVec(v1, v2))
    {}

    BoundingBox(float width, float height, const Vec2& c = Vec2(0, 0))
        : bottomLeft(c.x - 0.5f * width, c.y - 0.5f * height)
        , topRight(c.x + 0.5f * width, c.y + 0.5f * height)
    {}

    bool isValid() const
    {
        return bottomLeft.x <= topRight.x && bottomLeft.y <= topRight.y;
    }

    float width() const { return topRight.x - bottomLeft.x; }
    float height() const { return topRight.y - bottomLeft.y; }
    float area() const { return width() * height(); }

    bool contains(const Vec2& v) const
    {
        return bottomLeft.x <= v.x
            && bottomLeft.y <= v.y
            && v.x <= topRight.x
            && v.y <= topRight.y;
    }

    BoundingBox& enlarge(const Vec2& v)
    {
        bottomLeft = minVec(bottomLeft, v);
        topRight = maxVec(topRight, v);
        return *this;
    }

    BoundingBox& enlarge(const BoundingBox& other)
    {
        bottomLeft = minVec(bottomLeft, other.bottomLeft);
        topRight = maxVec(topRight, other.topRight);
        return *this;
    }

    BoundingBox& move(const Vec2& v)
    {
        bottomLeft += v;
        topRight += v;
        return *this;
    }

    BoundingBox moved(const Vec2& v) const
    {
        BoundingBox result(*this);
        result.bottomLeft += v;
        result.topRight += v;
        return result;
    }

    BoundingBox uniteWith(const BoundingBox& other) const
    {
        BoundingBox result;
        result.bottomLeft = minVec(bottomLeft, other.bottomLeft);
        result.topRight = maxVec(topRight, other.topRight);
        return result;
    }

    BoundingBox intersectWith(const BoundingBox& other) const
    {
        BoundingBox result;
        result.bottomLeft = maxVec(bottomLeft, other.bottomLeft);
        result.topRight = minVec(topRight, other.topRight);
        return result;
    }

    bool intersects(const BoundingBox& other) const
    {
        return bottomLeft.x <= other.topRight.x
            && bottomLeft.y <= other.topRight.y
            && topRight.x >= other.bottomLeft.x
            && topRight.y >= other.bottomLeft.y;
    }

    BoundingBox extension(float ext) const
    {
        BoundingBox result;
        result.bottomLeft = bottomLeft - Vec2(ext, ext);
        result.topRight = topRight + Vec2(ext, ext);
        return result;
    }

    BoundingBox transformed(const Transform2& transform) const
    {
        Vec2 points[] = {
            bottomLeft, Vec2(bottomLeft.x, topRight.y),
            topRight, Vec2(topRight.x, bottomLeft.y) };

        BoundingBox result;
        for (size_t i = 0; i < 4; ++i)
            result.enlarge(transform * points[i]);
        return result;
    }

    BoundingBox& transform(const Transform2& tr)
    {
        *this = transformed(tr);
        return *this;
    }

    Vec2 center() const
    {
        return 0.5f * (bottomLeft + topRight);
    }

    Vec2 bottomLeft;
    Vec2 topRight;
};

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

}
