/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/Gradient.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Gradient {
public:
    void setSizes(float width, float height);

    Color color1() const;
    void setColor1(float r, float g, float b, float a = 1);
    void setColor1(const Color& color);

    Color color2() const;
    void setColor2(float r, float g, float b, float a = 1);
    void setColor2(const Color& color);

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    Vec2 pos() const;
    void setPos(float x, float y);
    void setPos(const Vec2& v);
    BoundingBox box() const;
    BoundingBox movedBox() const;
    float width() const;
    float height() const;

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL(Gradient, Gradient);
};

/////////////// IMPLEMENTATION ///////////////////

inline void Gradient::setSizes(float width, float height) { m_impl->setFixedBox(width, height); }
inline Color Gradient::color1() const { return m_impl->color1(); }
inline void Gradient::setColor1(float r, float g, float b, float a) { m_impl->setColor1(Color(r, g, b, a)); }
inline void Gradient::setColor1(const Color& color) { m_impl->setColor1(color); }
inline Color Gradient::color2() const { return m_impl->color2(); }
inline void Gradient::setColor2(float r, float g, float b, float a) { m_impl->setColor2(Color(r, g, b, a)); }
inline void Gradient::setColor2(const Color& color) { m_impl->setColor2(color); }
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Gradient);

}
