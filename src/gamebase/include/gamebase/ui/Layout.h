/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/drawobj/DrawObj.h>
#include <gamebase/impl/adapt/ILayoutAdapter.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Layout {
public:
    template <typename T> T load(const std::string& fileName);
    template <typename T> T load(int id, const std::string& fileName);
    template <typename T> bool has(const T& obj) const;
    template <typename T> void remove(const T& obj);
    template <typename T> T get(int id) const;
    template <typename T> std::vector<T> all() const;
    template <typename T> T child(const std::string& name) const;
    template <typename T> int add(const T& obj);
    template <typename T> void insert(int id, const T& obj);

    DrawObj get(int id) const;
    std::vector<DrawObj> all() const;
    DrawObj load(const std::string& fileName);
    DrawObj load(int id, const std::string& fileName);
    bool has(int id) const;
    void remove(int id);
    void clear();
    void update();
    size_t size() const;
    bool empty() const;
    void setSize(float width, float height);
    void setSize(const Vec2& size);

    template <typename T> T cast() const;

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

    GAMEBASE_DEFINE_PIMPL_STD_SP(Layout, ILayoutAdapter);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline T Layout::load(const std::string& fileName)
{
    auto objImpl = impl::deserializeObj(fileName);
    m_impl->addObject(objImpl);
    return impl::wrap<T>(objImpl.get());
}
template <typename T> inline T Layout::load(int id, const std::string& fileName)
{
    auto objImpl = impl::deserializeObj(fileName);
    m_impl->insertObject(id, objImpl);
    return impl::wrap<T>(objImpl.get());
}
template <typename T> inline int Layout::add(const T& obj) { return m_impl->addObject(impl::unwrapShared(obj)); }
template <typename T> inline void Layout::insert(int id, const T& obj) { m_impl->insertObject(id, impl::unwrapShared(obj)); }
template <typename T> inline bool Layout::has(const T& obj) const { return m_impl->hasObject(impl::unwrapRaw(obj).get()); }
template <typename T> inline void Layout::remove(const T& obj) { m_impl->removeObject(impl::unwrapRaw(obj)); }
template <typename T> inline T Layout::get(int id) const { return impl::wrap<T>(m_impl->getIObject(id)); }
template <typename T> inline std::vector<T> Layout::all() const { return impl::wrap<T>(m_impl->objects()); }
template <typename T> inline T Layout::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline DrawObj Layout::get(int id) const { return get<DrawObj>(id); }
inline std::vector<DrawObj> Layout::all() const { return all<DrawObj>(); }
inline DrawObj Layout::load(const std::string& fileName) { return load<DrawObj>(fileName); }
inline DrawObj Layout::load(int id, const std::string& fileName) { return load<DrawObj>(id, fileName); }
inline bool Layout::has(int id) const { return m_impl->hasObject(id); }
inline void Layout::remove(int id) { m_impl->removeObject(id); }
inline void Layout::clear() { m_impl->clear(); }
inline void Layout::update() { m_impl->update(); }
inline size_t Layout::size() const { return m_impl->objects().size(); }
inline bool Layout::empty() const { return size() == 0; }
inline void Layout::setSize(float width, float height) { m_impl->setFixedBox(width, height); }
inline void Layout::setSize(const Vec2& size) { m_impl->setFixedBox(size.x, size.y); }
template <typename T> inline T Layout::cast() const { return impl::wrap<T>(impl::unwrapSmart(*this)); }
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Layout);

}
