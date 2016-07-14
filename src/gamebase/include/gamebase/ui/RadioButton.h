/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/ui/RadioButton.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class RadioButton {
public:
    template <typename T> T child(const std::string& name) const;

    int id() const;

    bool isChecked() const;
    void check();

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

    bool isEnabled() const;
    void setEnabled(bool value);
    void enable();
    void disable();

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL(RadioButton, RadioButton);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline T RadioButton::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline int RadioButton::id() const { return m_impl->indexInGroup(); }
inline bool RadioButton::isChecked() const { return m_impl->isChecked(); }
inline void RadioButton::check() { m_impl->setChecked(); }
GAMEBASE_DEFINE_UI_ACTIVE_ELEMENT_METHODS(RadioButton);

}
