/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/geom/BoundingBox.h>
#include <gamebase/math/Vector2.h>
#include <gamebase/math/Transform2.h>
#include <iostream>

namespace gamebase {

struct Box {
    Box();
    Box(float x, float y);
    Box(const Vec2& v);
    Box(float x1, float y1, float x2, float y2);
    Box(const Vec2& v1, const Vec2& v2);
    Box(const impl::BoundingBox& boxImpl);

    float l;
    float r;
    float b;
    float t;
    float w;
    float h;

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
    template <typename T> bool intersects(const T& obj) const;
    bool intersects(const Box& other) const;

    Box& add(const Vec2& v);
    Box& add(float x, float y);
    Box& add(const Box& other);

    Box& move(const Vec2& v);
    Box& move(float x, float y);

    Box& extend(float ext);
    Box& extend(float extX, float extY);

    Box& scale(float scale);
    Box& scale(float scaleX, float scaleY);

    Box& transform(const Transform2& tr);

    Vec2 leftBottom() const;
    Vec2 rightBottom() const;
    Vec2 leftTop() const;
    Vec2 rightTop() const;

    void update();
};

inline Box unite(const Box& b1, const Box& b2);
inline Box intersect(const Box& b1, const Box& b2);
inline Box around(float w, float h, const Vec2& center);

inline bool operator==(const Box& b1, const Box& b2);
inline bool operator!=(const Box& b1, const Box& b2);
inline std::ostream& operator<<(std::ostream& stream, const Box& box);

/////////////// IMPLEMENTATION ///////////////////

inline Box::Box()
    : l(std::numeric_limits<float>::max())
    , r(std::numeric_limits<float>::lowest())
    , b(std::numeric_limits<float>::max())
    , t(std::numeric_limits<float>::lowest())
    , w(std::numeric_limits<float>::lowest())
    , h(std::numeric_limits<float>::lowest())
{}

inline Box::Box(const Vec2& v)
    : l(v.x)
    , r(v.x)
    , b(v.y)
    , t(v.y)
    , w(0)
    , h(0)
{}

inline Box::Box(float x, float y)
    : l(x)
    , r(x)
    , b(y)
    , t(y)
    , w(0)
    , h(0)
{}

inline Box::Box(float x1, float y1, float x2, float y2)
    : l(std::min(x1, x2))
    , r(std::max(x1, x2))
    , b(std::min(y1, y2))
    , t(std::max(y1, y2))
    , w(r - l)
    , h(t - b)
{}

inline Box::Box(const Vec2& v1, const Vec2& v2)
    : l(std::min(v1.x, v2.x))
    , r(std::max(v1.x, v2.x))
    , b(std::min(v1.y, v2.y))
    , t(std::max(v1.y, v2.y))
    , w(r - l)
    , h(t - b)
{}

inline Box::Box(const impl::BoundingBox& boxImpl)
    : l(boxImpl.bottomLeft.x)
    , r(boxImpl.topRight.x)
    , b(boxImpl.bottomLeft.y)
    , t(boxImpl.topRight.y)
    , w(r - l)
    , h(t - b)
{}

inline float Box::left() const { return l; }
inline float Box::right() const { return r; }
inline float Box::bottom() const { return b; }
inline float Box::top() const { return t; }

inline bool Box::isValid() const
{
    return l <= r && b <= t;
}

inline float Box::width() const { return w; }
inline float Box::height() const { return h; }
inline Vec2 Box::size() const { return Vec2(w, h); }
inline float Box::area() const { return w * h; }

inline Vec2 Box::center() const
{
    return Vec2(l + 0.5f * w, b + 0.5f * h);
}

inline bool Box::contains(const Vec2& v) const
{
    return l <= v.x && v.x <= r
        && b <= v.y && v.y <= t;
}

template <typename T> inline bool Box::intersects(const T& obj) const
{
    return intersects(obj.box());
}

inline bool Box::intersects(const Box& other) const
{
    return l <= other.r
        && b <= other.t
        && r >= other.l
        && t >= other.b;
}

inline Box& Box::add(const Vec2& v)
{
    l = std::min(l, v.x);
    r = std::max(r, v.x);
    b = std::min(b, v.y);
    t = std::max(t, v.y);
    update();
    return *this;
}

inline Box& Box::add(float x, float y)
{
    return add(Vec2(x, y));
}

inline Box& Box::add(const Box& other)
{
    l = std::min(l, other.l);
    r = std::max(r, other.r);
    b = std::min(b, other.b);
    t = std::max(t, other.t);
    update();
    return *this;
}

inline Box& Box::move(const Vec2& v)
{
    l += v.x;
    r += v.x;
    b += v.y;
    t += v.y;
    return *this;
}

inline Box& Box::move(float x, float y)
{
    return move(Vec2(x, y));
}

inline Box& Box::extend(float ext)
{
    return extend(ext, ext);
}

inline Box& Box::extend(float extX, float extY)
{
    l -= extX;
    r += extX;
    b -= extY;
    t += extY;
    update();
    return *this;
}

inline Box& Box::scale(float s)
{
    return scale(s, s);
}

inline Box& Box::scale(float sx, float sy)
{
    *this = around(w * sx, h * sy, center());
    return *this;
}

inline Box& Box::transform(const Transform2& tr)
{
    impl::BoundingBox boxImpl;
    boxImpl.bottomLeft = Vec2(l, b);
    boxImpl.topRight = Vec2(r, t);
    boxImpl.transform(tr);
    *this = Box(boxImpl);
    return *this;
}

inline Vec2 Box::leftBottom() const
{
    return Vec2(l, b);
}

inline Vec2 Box::rightBottom() const
{
    return Vec2(r, b);
}

inline Vec2 Box::leftTop() const
{
    return Vec2(l, t);
}

inline Vec2 Box::rightTop() const
{
    return Vec2(r, t);
}

inline Box unite(const Box& b1, const Box& b2)
{
    Box result;
    result.l = std::min(b1.l, b2.l);
    result.r = std::max(b1.r, b2.r);
    result.b = std::min(b1.b, b2.b);
    result.t = std::max(b1.t, b2.t);
    result.update();
    return result;
}

inline Box intersect(const Box& b1, const Box& b2)
{
    Box result;
    result.l = std::max(b1.l, b2.l);
    result.r = std::min(b1.r, b2.r);
    result.b = std::max(b1.b, b2.b);
    result.t = std::min(b1.t, b2.t);
    result.update();
    return result;
}

inline Box around(float w, float h, const Vec2& center)
{
    float halfWidth = 0.5f * w;
    float halfHeight = 0.5f * h;
    return Box(
        center.x - halfWidth, center.y - halfHeight,
        center.x + halfWidth, center.y + halfHeight);
}

inline bool operator==(const Box& b1, const Box& b2)
{
    return b1.l == b2.l && b1.r == b2.r && b1.b == b2.b && b1.t == b2.t;
}

inline bool operator!=(const Box& b1, const Box& b2)
{
    return !(b1 == b2);
}

inline std::ostream& operator<<(std::ostream& stream, const Box& box)
{
    stream << box.leftBottom() << " x " << box.rightTop();
    return stream;
}

inline void Box::update()
{
    w = r - l;
    h = t - b;
}

}
