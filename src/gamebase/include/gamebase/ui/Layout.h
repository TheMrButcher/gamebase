#pragma once

#include <gamebase/impl/adapt/ILayoutAdapter.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Layout {
public:
    template <typename T> void add(const T& obj);
    template <typename T> std::vector<T> all() const;
    template <typename T> T child(const std::string& name) const;

    void clear();
    void update();

    void setSizes(float width, float height);

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    Vec2 pos() const;
    void setPos(float x, float y);
    void setPos(const Vec2& v);
    BoundingBox box() const;
    BoundingBox movedBox() const;

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL_STD_SP(Layout, ILayoutAdapter);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline void Layout::add(const T& obj) { m_impl->addObject(impl::unwrapShared(obj)); }
template <typename T> inline std::vector<T> Layout::all() const { return impl::wrap<T>(m_impl->objects()); }
template <typename T> inline T Layout::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline void Layout::clear() { m_impl->clear(); }
inline void Layout::update() { m_impl->update(); }
inline void Layout::setSizes(float width, float height) { m_impl->setFixedBox(width, height); }
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Layout);

}
