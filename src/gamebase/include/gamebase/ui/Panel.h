/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/ui/Panel.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Panel {
public:
    template <typename T> T child(const std::string& name) const;
    
    void update();
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

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL(Panel, Panel);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline T Panel::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline void Panel::update() { m_impl->resetPosition(); }
inline void Panel::setCallback(const std::function<void()>& callback) { m_impl->setCloseCallback(callback); }
inline bool Panel::isVisible() const { return m_impl->isVisible(); }
inline void Panel::setVisible(bool value) { if (value) show(); else hide(); }
inline void Panel::show() { m_impl->setVisible(true); }
inline void Panel::hide() { m_impl->close(); }
inline Vec2 Panel::pos() const { return m_impl->position().offset; }
inline void Panel::setPos(const Vec2& v) { m_impl->resetPosition(); m_impl->setOffset(v); }
inline Box Panel::box() const { return m_impl->transformedBox(); }
inline float Panel::width() const { return box().width(); }
inline float Panel::height() const { return box().height(); }

}
