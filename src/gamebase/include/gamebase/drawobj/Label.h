/**
 * Copyright (c) 2016 Slavnejshev Filipp
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

    Color color() const;
    void setColor(float r, float g, float b, float a = 1);
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
GAMEBASE_DEFINE_TEXT_METHODS(Label);
GAMEBASE_DEFINE_COLOR_METHODS(Label);
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Label);

}
