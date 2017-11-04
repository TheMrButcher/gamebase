/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/StaticLabel.h>
#include <gamebase/impl/pubhelp/Helpers.h>
#include <gamebase/tools/Writer.h>

namespace gamebase {

class Label {
public:
    const std::string& text() const;
    void setText(const std::string& text);

	template <typename T> Writer operator<<(const T& value) const;

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

    GAMEBASE_DEFINE_PIMPL(Label, StaticLabel);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> Writer Label::operator<<(const T& value) const { return Writer(*this) << value; }
inline void Label::setSize(float width, float height) { m_impl->setFixedBox(width, height); }
inline void Label::setSize(const Vec2& size) { m_impl->setFixedBox(size.x, size.y); }
GAMEBASE_DEFINE_TEXT_METHODS(Label);
GAMEBASE_DEFINE_COLOR_METHODS(Label);
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Label);

}
