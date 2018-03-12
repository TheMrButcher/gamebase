/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/ui/ToggleButton.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class ToggleButton {
public:
    template <typename T> T child(const std::string& name) const;

    void setCallback(const std::function<void()>& callback);

    bool isPressed() const;
    void setPressed(bool value);
    void press();
    void unpress();

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

    GAMEBASE_DEFINE_PIMPL(ToggleButton, ToggleButton);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline T ToggleButton::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline void ToggleButton::setCallback(const std::function<void()>& callback) { m_impl->setCallback(callback); }
inline bool ToggleButton::isPressed() const { return m_impl->isPressed(); }
inline void ToggleButton::setPressed(bool value) { m_impl->setPressed(value); }
inline void ToggleButton::press() { m_impl->setPressed(true); }
inline void ToggleButton::unpress() { m_impl->setPressed(false); }
GAMEBASE_DEFINE_UI_ACTIVE_ELEMENT_METHODS(ToggleButton);

}
