/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/gameview/Layer.h>

namespace gamebase {

template <>
class Layer<void> {
public:
    template <typename T> bool has(const T& obj) const;
    template <typename T> void remove(const T& obj);
    template <typename T> T get(int id) const;
    template <typename T> std::vector<T> all() const;
    template <typename T> std::vector<T> find(const Box& box) const;
    template <typename T> std::vector<T> find(const Vec2& v) const;
    template <typename T> std::vector<T> find(float x, float y) const;
    template <typename T> std::vector<T> find(const GameObj& obj) const;
    template <typename T> T child(const std::string& name) const;
    template <typename T> int add(const T& obj);
    template <typename T> void insert(int id, const T& obj);

    GameObj get(int id) const;
    std::vector<GameObj> all() const;
    std::vector<GameObj> find(const Box& box) const;
    std::vector<GameObj> find(const Vec2& v) const;
    std::vector<GameObj> find(float x, float y) const;
    std::vector<GameObj> find(const GameObj& obj) const;
    GameObj child(const std::string& name) const;

    int id() const;
    std::string name() const;

    bool has(int id) const;
    void remove(int id);
    void clear();
    void update();
    size_t size() const;
    bool empty() const;

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    operator bool() const;

    template <typename T> T load(const std::string& fileName);
    template <typename T> T load(const std::string& fileName, float x, float y);
    template <typename T> T load(const std::string& fileName, const Vec2& pos);
    template <typename T> T load(int id, const std::string& fileName);
    template <typename T> T load(int id, const std::string& fileName, float x, float y);
    template <typename T> T load(int id, const std::string& fileName, const Vec2& pos);
    GameObj load(const std::string& fileName);
    GameObj load(const std::string& fileName, float x, float y);
    GameObj load(const std::string& fileName, const Vec2& pos);
    GameObj load(int id, const std::string& fileName);
    GameObj load(int id, const std::string& fileName, float x, float y);
    GameObj load(int id, const std::string& fileName, const Vec2& pos);

    GAMEBASE_DEFINE_PIMPL(Layer, ILayer);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline int Layer<void>::add(const T& obj) { return m_impl->addObject(impl::unwrapShared(obj)); }
template <typename T> inline void Layer<void>::insert(int id, const T& obj) { m_impl->insertObject(id, impl::unwrapShared(obj)); }
template <typename T> inline bool Layer<void>::has(const T& obj) const { return m_impl->hasObject(impl::unwrapRaw(obj).get()); }
template <typename T> inline void Layer<void>::remove(const T& obj) { m_impl->removeObject(impl::unwrapRaw(obj)); }
template <typename T> inline T Layer<void>::get(int id) const { return impl::wrap<T>(m_impl->getIObject(id)); }
template <typename T> inline std::vector<T> Layer<void>::all() const { return impl::wrap<T>(m_impl->getIObjects()); }
template <typename T> inline std::vector<T> Layer<void>::find(const Box& box) const { return impl::wrap<T>(m_impl->findByBox(impl::wrap(box))); }
template <typename T> inline std::vector<T> Layer<void>::find(const Vec2& v) const { return find<T>(Box(v)); }
template <typename T> inline std::vector<T> Layer<void>::find(float x, float y) const { return find<T>(Box(x, y)); }
template <typename T> inline std::vector<T> Layer<void>::find(const GameObj& obj) const { return find<T>(obj.box()); }
template <typename T> inline T Layer<void>::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline GameObj Layer<void>::get(int id) const { return impl::wrap<GameObj>(m_impl->getIObject(id)); }
inline std::vector<GameObj> Layer<void>::all() const { return impl::wrap<GameObj>(m_impl->getIObjects()); }
inline std::vector<GameObj> Layer<void>::find(const Box& box) const { return impl::wrap<GameObj>(m_impl->findByBox(impl::wrap(box))); }
inline std::vector<GameObj> Layer<void>::find(const Vec2& v) const { return find(Box(v)); }
inline std::vector<GameObj> Layer<void>::find(float x, float y) const { return find(Box(x, y)); }
inline std::vector<GameObj> Layer<void>::find(const GameObj& obj) const { return find(obj.box()); }
inline GameObj Layer<void>::child(const std::string& name) const { return impl::findAndWrap<GameObj>(m_impl.get(), name); }
inline int Layer<void>::id() const { return m_impl->id(); }
inline std::string Layer<void>::name() const { return m_impl->name(); }
inline bool Layer<void>::has(int id) const { return m_impl->hasObject(id); }
inline void Layer<void>::remove(int id) { m_impl->removeObject(id); }
inline void Layer<void>::clear() { m_impl->clear(); }
inline void Layer<void>::update() { m_impl->update(); }
inline size_t Layer<void>::size() const { return m_impl->size(); }
inline bool Layer<void>::empty() const { return size() == 0; }
inline bool Layer<void>::isVisible() const { return m_impl->isVisible(); }
inline void Layer<void>::setVisible(bool value) { m_impl->setVisible(value); }
inline void Layer<void>::show() { m_impl->setVisible(true); }
inline void Layer<void>::hide() { m_impl->setVisible(false); }
inline Layer<void>::operator bool() const { return m_impl; }
template <typename T> inline T Layer<void>::load(const std::string& fileName)
{
    auto objImpl = impl::deserializeObj(fileName);
    m_impl->addObject(objImpl);
    return impl::wrap<T>(objImpl.get());
}
template <typename T> inline T Layer<void>::load(const std::string& fileName, float x, float y) { return load<T>(fileName, Vec2(x, y)); }
template <typename T> inline T Layer<void>::load(const std::string& fileName, const Vec2& pos) { auto result = load<T>(fileName); result.setPos(pos); return result; }
template <typename T> inline T Layer<void>::load(int id, const std::string& fileName)
{
    auto objImpl = impl::deserializeObj(fileName);
    m_impl->insertObject(id, objImpl);
    return impl::wrap<T>(objImpl.get());
}
template <typename T> inline T Layer<void>::load(int id, const std::string& fileName, float x, float y) { return load<T>(id, fileName, Vec2(x, y)); }
template <typename T> inline T Layer<void>::load(int id, const std::string& fileName, const Vec2& pos) { auto result = load<T>(id, fileName); result.setPos(pos); return result; }
inline GameObj Layer<void>::load(const std::string& fileName) { return load<GameObj>(fileName); }
inline GameObj Layer<void>::load(const std::string& fileName, float x, float y) { return load<GameObj>(fileName, x, y); }
inline GameObj Layer<void>::load(const std::string& fileName, const Vec2& pos) { return load<GameObj>(fileName, pos); }
inline GameObj Layer<void>::load(int id, const std::string& fileName) { return load<GameObj>(id, fileName); }
inline GameObj Layer<void>::load(int id, const std::string& fileName, float x, float y) { return load<GameObj>(id, fileName, x, y); }
inline GameObj Layer<void>::load(int id, const std::string& fileName, const Vec2& pos) { return load<GameObj>(id, fileName, pos); }

}
