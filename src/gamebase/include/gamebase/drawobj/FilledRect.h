/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/StaticFilledRect.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class FilledRect {
public:
    void setSize(float width, float height);
    void setSize(const Vec2& size);

    Color color() const;
    void setColor(int r, int g, int b, int a = 255);
    void setColor(const Color& color);

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

    GAMEBASE_DEFINE_PIMPL(FilledRect, StaticFilledRect);
};

/////////////// IMPLEMENTATION ///////////////////

inline void FilledRect::setSize(float width, float height) { m_impl->setFixedBox(width, height); }
inline void FilledRect::setSize(const Vec2& size) { m_impl->setFixedBox(size.x, size.y); }
GAMEBASE_DEFINE_COLOR_METHODS(FilledRect);
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(FilledRect);

}
