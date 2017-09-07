/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/ui/Button.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Button {
public:
    template <typename T> T child(const std::string& name) const;

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

    GAMEBASE_DEFINE_PIMPL(Button, Button);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline T Button::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline void Button::setCallback(const std::function<void()>& callback) { m_impl->setCallback(callback); }
GAMEBASE_DEFINE_UI_ACTIVE_ELEMENT_METHODS(Button);

}
