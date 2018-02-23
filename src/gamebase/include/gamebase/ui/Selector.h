/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/drawobj/DrawObj.h>
#include <gamebase/impl/ui/SelectingWidget.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Selector {
public:
    template <typename T> int add(const T& obj);
    template <typename T> void insert(int id, const T& obj);
    template <typename T> bool has(const T& obj) const;
    template <typename T> void remove(const T& obj);
    template <typename T> T get(int id) const;
    template <typename T> std::vector<T> all() const;
    template <typename T> T child(const std::string& name) const;

    DrawObj get(int id) const;
    std::vector<DrawObj> all() const;
    template <typename T> T load(int id, const std::string& fileName);
    DrawObj load(int id, const std::string& fileName);

    void select(int id);
    int selected() const;

    bool has(int id) const;
    void remove(int id);
    void clear();
    void update();
    size_t size() const;
    bool empty() const;
    void setSize(float width, float height);
	void setSize(const Vec2& size);

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

    GAMEBASE_DEFINE_PIMPL(Selector, SelectingWidget);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline int Selector::add(const T& obj) { return m_impl->addObject(impl::unwrapShared(obj)); }
template <typename T> inline void Selector::insert(int id, const T& obj) { m_impl->insertObject(id, impl::unwrapShared(obj)); }
template <typename T> inline bool Selector::has(const T& obj) const { return m_impl->hasObject(impl::unwrapRaw(obj).get()); }
template <typename T> inline void Selector::remove(const T& obj) { m_impl->removeObject(impl::unwrapRaw(obj)); }
template <typename T> inline T Selector::get(int id) const { return impl::wrap<T>(m_impl->getIObject(id)); }
template <typename T> inline std::vector<T> Selector::all() const { return impl::wrap<T>(m_impl->objects()); }
template <typename T> inline T Selector::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline DrawObj Selector::get(int id) const { return get<DrawObj>(id); }
inline std::vector<DrawObj> Selector::all() const { return all<DrawObj>(); }
template <typename T> inline T Selector::load(int id, const std::string& fileName)
{
    auto objImpl = impl::deserialize<impl::IObject>(fileName);
    m_impl->insertObject(id, objImpl);
    return impl::wrap<T>(objImpl.get());
}
inline DrawObj Selector::load(int id, const std::string& fileName) { return load<DrawObj>(id, fileName); }
inline void Selector::select(int id) { m_impl->select(id); }
inline int Selector::selected() const { return m_impl->selected(); }
inline bool Selector::has(int id) const { return m_impl->hasObject(id); }
inline void Selector::remove(int id) { m_impl->removeObject(id); }
inline void Selector::clear() { m_impl->clear(); }
inline void Selector::update() { m_impl->markAllNeedReload(); select(selected()); }
inline size_t Selector::size() const { return m_impl->objectsAsList().size(); }
inline bool Selector::empty() const { return size() == 0; }
inline void Selector::setSize(float width, float height) { m_impl->setFixedBox(width, height); }
inline void Selector::setSize(const Vec2& size) { m_impl->setFixedBox(size.x, size.y); }
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Selector);

}
