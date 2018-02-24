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

    std::string font() const;
    void setFont(const std::string& font);

    float fontSize() const;
    void setFontSize(float fontSize);

    float outlineWidth() const;
    void setOutlineWidth(float width);

    Color color() const;
    void setColor(int r, int g, int b, int a = 255);
    void setColor(const Color& color);

    Color outlineColor() const;
    void setOutlineColor(int r, int g, int b, int a = 255);
    void setOutlineColor(const Color& color);

    bool bold() const;
    bool italic() const;
    bool underlined() const;
    bool strikeout() const;
    void setBold(bool value);
    void setItalic(bool value);
    void setUnderlined(bool value);
    void setStrikeout(bool value);

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
inline std::string Label::font() const { return m_impl->alignProperties().font.fontFamily; }
inline void Label::setFont(const std::string& font) { auto props = m_impl->alignProperties(); props.font.fontFamily = font; m_impl->setAlignProperties(props); }
inline float Label::fontSize() const { return m_impl->alignProperties().font.size; }
inline void Label::setFontSize(float fontSize) { auto props = m_impl->alignProperties(); props.font.size = fontSize; m_impl->setAlignProperties(props); }
inline float Label::outlineWidth() const { return m_impl->alignProperties().font.outlineWidth; }
inline void Label::setOutlineWidth(float width) { auto props = m_impl->alignProperties(); props.font.outlineWidth = width; m_impl->setAlignProperties(props); }
inline Color Label::outlineColor() const { return m_impl->outlineColor().intColor(); }
inline void Label::setOutlineColor(int r, int g, int b, int a) { m_impl->setOutlineColor(impl::makeGLColor(r, g, b, a)); }
inline void Label::setOutlineColor(const Color& color) { m_impl->setOutlineColor(impl::makeGLColor(color)); }
inline bool Label::bold() const { return m_impl->alignProperties().font.bold; }
inline bool Label::italic() const { return m_impl->alignProperties().font.italic; }
inline bool Label::underlined() const { return m_impl->alignProperties().font.underlined; }
inline bool Label::strikeout() const { return m_impl->alignProperties().font.lineThrough; }
inline void Label::setBold(bool value) { auto props = m_impl->alignProperties(); props.font.bold = value; m_impl->setAlignProperties(props); }
inline void Label::setItalic(bool value) { auto props = m_impl->alignProperties(); props.font.italic = value; m_impl->setAlignProperties(props); }
inline void Label::setUnderlined(bool value) { auto props = m_impl->alignProperties(); props.font.underlined = value; m_impl->setAlignProperties(props); }
inline void Label::setStrikeout(bool value) { auto props = m_impl->alignProperties(); props.font.lineThrough = value; m_impl->setAlignProperties(props); }
GAMEBASE_DEFINE_TEXT_METHODS(Label);
GAMEBASE_DEFINE_COLOR_METHODS(Label);
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Label);

}
