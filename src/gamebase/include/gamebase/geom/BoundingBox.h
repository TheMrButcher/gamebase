#pragma once

#include <gamebase/math/Vector2.h>

namespace gamebase {

struct BoundingBox {
    BoundingBox()
        : bottomLeft(
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max())
        , topRight(
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min())
    {}

    BoundingBox(const Vec2& v)
        : bottomLeft(v)
        , topRight(v)
    {}

    BoundingBox(const Vec2& v1, const Vec2& v2)
        : bottomLeft(minVec(v1, v2))
        , topRight(maxVec(v1, v2))
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

    BoundingBox& extend(const Vec2& v)
    {
        bottomLeft = minVec(bottomLeft, v);
        topRight = maxVec(topRight, v);
        return *this;
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

    Vec2 bottomLeft;
    Vec2 topRight;
};

}
