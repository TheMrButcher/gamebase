/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/Line.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Line {
public:
    template <typename T> T child(const std::string& name) const;

    const Vec2& p1() const;
    void setP1(const Vec2& v);
    void setP1(float x, float y);

    const Vec2& p2() const;
    void setP2(const Vec2& v);
    void setP2(float x, float y);

    float width() const;
    void setWidth(float value);

    void set(const Vec2& p1, const Vec2& p2, float width);
    void set(const Vec2& p1, const Vec2& p2);

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();
    bool isMouseOn() const;

    Box box() const;

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL(Line, Line);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline T Line::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline const Vec2& Line::p1() const { return m_impl->p1(); }
inline void Line::setP1(const Vec2& v) { m_impl->setP1(v); }
inline void Line::setP1(float x, float y) { m_impl->setP1(Vec2(x, y)); }
inline const Vec2& Line::p2() const { return m_impl->p2(); }
inline void Line::setP2(const Vec2& v) { m_impl->setP2(v); }
inline void Line::setP2(float x, float y) { m_impl->setP2(Vec2(x, y)); }
inline float Line::width() const { return m_impl->width(); }
inline void Line::setWidth(float value) { m_impl->setWidth(value); }
inline void Line::set(const Vec2& p1, const Vec2& p2, float width) { m_impl->set(p1, p2, width); }
inline void Line::set(const Vec2& p1, const Vec2& p2) { m_impl->set(p1, p2); }
GAMEBASE_DEFINE_DRAWABLE_METHODS(Line);
inline Box Line::box() const { return m_impl->transformedBox(); }

}
