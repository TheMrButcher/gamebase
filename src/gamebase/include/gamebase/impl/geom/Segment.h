#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/math/Vector2.h>
#include <gamebase/geom/BoundingBox.h>
#include <boost/optional.hpp>

namespace gamebase { namespace impl {

struct Segment {
    Segment(const Vec2& a, const Vec2& b)
        : a(a)
        , b(b)
    {}

    BoundingBox boundingBox() const
    {
        return BoundingBox(a, b);
    }

    bool isVertical() const { return b.x - a.x == 0; }

    bool isHorizontal() const { return b.y - a.y == 0; }

    float k() const
    {
        if (isVertical())
            THROW_EX() << "Can't count k for vertical segment";
        Vec2 v = b - a;
        return v.y / v.x;
    }

    float c() const
    {
        return a.y - k() * a.x;
    }

    float getX(float y) const
    {
        if (isHorizontal())
            THROW_EX() << "Can't get x for horizontal segment";
        return (y - c()) / k();
    }

    float substitute(float x) const
    {
        if (isVertical())
            THROW_EX() << "Can't substitute x for vertical segment";
        return k() * x + c();
    }

    Vec2 a;
    Vec2 b;
};

boost::optional<Vec2> GAMEBASE_API intersection(
    const Segment& s1, const Segment& s2);

} }
