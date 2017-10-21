/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/Gradient.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Gradient {
public:
    void setSize(float width, float height);
	void setSize(const Vec2& size);

    Color color1() const;
    void setColor1(int r, int g, int b, int a = 255);
    void setColor1(const Color& color);

    Color color2() const;
    void setColor2(int r, int g, int b, int a = 255);
    void setColor2(const Color& color);

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    Vec2 pos() const;
    void setPos(float x, float y);
    void setPos(const Vec2& v);
    Box box() const;
    float width() const;
    float height() const;

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL(Gradient, Gradient);
};

/////////////// IMPLEMENTATION ///////////////////

inline void Gradient::setSize(float width, float height) { m_impl->setFixedBox(width, height); }
inline void Gradient::setSize(const Vec2& size) { m_impl->setFixedBox(size.x, size.y); }
inline Color Gradient::color1() const { return m_impl->color1().intColor(); }
inline void Gradient::setColor1(int r, int g, int b, int a) { m_impl->setColor1(impl::makeGLColor(r, g, b, a)); }
inline void Gradient::setColor1(const Color& color) { m_impl->setColor1(impl::makeGLColor(color)); }
inline Color Gradient::color2() const { return m_impl->color2().intColor(); }
inline void Gradient::setColor2(int r, int g, int b, int a) { m_impl->setColor2(impl::makeGLColor(r, g, b, a)); }
inline void Gradient::setColor2(const Color& color) { m_impl->setColor2(impl::makeGLColor(color)); }
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Gradient);

}
