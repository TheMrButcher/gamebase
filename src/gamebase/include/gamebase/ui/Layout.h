/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/adapt/ILayoutAdapter.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Layout {
public:
    template <typename T> int add(const T& obj);
    template <typename T> void insert(int id, const T& obj);
    template <typename T> bool has(const T& obj) const;
    template <typename T> void remove(const T& obj);
    template <typename T> T get(int id) const;
    template <typename T> std::vector<T> all() const;
    template <typename T> T child(const std::string& name) const;

    bool has(int id) const;
    void remove(int id);
    void clear();
    void update();
    size_t size() const;
    bool empty() const;
    void setSizes(float width, float height);

    template <typename T> T cast() const;

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

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL_STD_SP(Layout, ILayoutAdapter);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline int Layout::add(const T& obj) { return m_impl->addObject(impl::unwrapShared(obj)); }
template <typename T> inline void Layout::insert(int id, const T& obj) { m_impl->insertObject(id, impl::unwrapShared(obj)); }
template <typename T> inline bool Layout::has(const T& obj) const { return m_impl->hasObject(impl::unwrapRaw(obj).get()); }
template <typename T> inline void Layout::remove(const T& obj) { m_impl->removeObject(impl::unwrapRaw(obj)); }
template <typename T> inline T Layout::get(int id) const { return impl::wrap<T>(m_impl->getIObject(id)); }
template <typename T> inline std::vector<T> Layout::all() const { return impl::wrap<T>(m_impl->objects()); }
template <typename T> inline T Layout::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline bool Layout::has(int id) const { return m_impl->hasObject(id); }
inline void Layout::remove(int id) { m_impl->removeObject(id); }
inline void Layout::clear() { m_impl->clear(); }
inline void Layout::update() { m_impl->update(); }
inline size_t Layout::size() const { return m_impl->objects().size(); }
inline bool Layout::empty() const { return size() == 0; }
inline void Layout::setSizes(float width, float height) { m_impl->setFixedBox(width, height); }
template <typename T> inline T Layout::cast() const { return impl::wrap<T>(impl::unwrapSmart(*this)); }
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Layout);

}
