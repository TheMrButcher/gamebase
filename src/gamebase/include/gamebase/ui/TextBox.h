/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/ui/TextBox.h>
#include <gamebase/impl/pubhelp/Helpers.h>
#include <gamebase/tools/Reader.h>
#include <gamebase/tools/Writer.h>

namespace gamebase {

class TextBox {
public:
    template <typename T> T child(const std::string& name) const;

	template <typename T> Reader operator>>(T& dst) const;
	template <typename T> Writer operator<<(const T& value) const;

    const std::string& text() const;
    void setText(const std::string& text);

    void setCallback(const std::function<void()>& callback);

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

    bool isEnabled() const;
    void setEnabled(bool value);
    void enable();
    void disable();

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL(TextBox, TextBox);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline T TextBox::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
template <typename T> Reader TextBox::operator>>(T& dst) const { return Reader(*this) >> dst; }
template <typename T> Writer TextBox::operator<<(const T& value) const { return Writer(*this) << value; }
GAMEBASE_DEFINE_TEXT_METHODS(TextBox);
inline void TextBox::setCallback(const std::function<void()>& callback) { m_impl->setCallback(callback); }
GAMEBASE_DEFINE_UI_ACTIVE_ELEMENT_METHODS(TextBox);

}
