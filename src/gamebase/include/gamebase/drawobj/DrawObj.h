/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pubhelp/DrawObjHelpers.h>

namespace gamebase {

class DrawObj {
public:
    template <typename T> T cast() const;
    template <typename T> T child(const std::string& name) const;

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();
    bool isMouseOn() const;

    Vec2 pos() const;
    void setPos(float x, float y);
    void setPos(const Vec2& v);
    Box box() const;
    float width() const;
    float height() const;

    operator bool() const;

    GAMEBASE_DEFINE_DRAW_OBJ_PIMPL();
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline T DrawObj::cast() const { return impl::wrap<T>(impl::unwrapSmart(*this)); }
template <typename T> inline T DrawObj::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(DrawObj);

}
