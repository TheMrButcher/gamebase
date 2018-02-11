/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/gameobj/GameObj.h>
#include <gamebase/impl/gameview/ILayer.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

template <typename DataType>
class Layer {
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
    template <typename T> DataType& data(const T& obj);
	template <typename T> int add(const T& obj);
	template <typename T> void insert(int id, const T& obj);

    GameObj get(int id) const;
    std::vector<GameObj> all() const;
    std::vector<GameObj> find(const Box& box) const;
    std::vector<GameObj> find(float x, float y) const;
    std::vector<GameObj> find(const Vec2& v) const;
    std::vector<GameObj> find(const GameObj& obj) const;
    GameObj child(const std::string& name) const;
    DataType& data(const GameObj& obj);
    DataType& data(int id);

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

template <typename DataType> template <typename T> inline int Layer<DataType>::add(const T& obj) { return m_impl->addObject(impl::unwrapShared(obj)); }
template <typename DataType> template <typename T> inline void Layer<DataType>::insert(int id, const T& obj) { m_impl->insertObject(id, impl::unwrapShared(obj)); }
template <typename DataType> template <typename T> inline bool Layer<DataType>::has(const T& obj) const { return m_impl->hasObject(impl::unwrapRaw(obj).get()); }
template <typename DataType> template <typename T> inline void Layer<DataType>::remove(const T& obj) { m_impl->removeObject(impl::unwrapRaw(obj)); }
template <typename DataType> template <typename T> inline T Layer<DataType>::get(int id) const { return impl::wrap<T>(m_impl->getIObject(id)); }
template <typename DataType> template <typename T> inline std::vector<T> Layer<DataType>::all() const { return impl::wrap<T>(m_impl->getIObjects()); }
template <typename DataType> template <typename T> inline std::vector<T> Layer<DataType>::find(const Box& box) const { return impl::wrap<T>(m_impl->findByBox(impl::wrap(box))); }
template <typename DataType> template <typename T> inline std::vector<T> Layer<DataType>::find(const Vec2& v) const { return find<T>(Box(v)); }
template <typename DataType> template <typename T> inline std::vector<T> Layer<DataType>::find(float x, float y) const { return find<T>(Box(x, y)); }
template <typename DataType> template <typename T> inline std::vector<T> Layer<DataType>::find(const GameObj& obj) const { return find<T>(obj.box()); }
template <typename DataType> template <typename T> inline T Layer<DataType>::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
template <typename DataType> template <typename T> inline DataType& Layer<DataType>::data(const T& obj) { return m_impl->data<DataType>(impl::unwrapRaw(obj)); }
template <typename DataType> inline GameObj Layer<DataType>::get(int id) const { return impl::wrap<GameObj>(m_impl->getIObject(id)); }
template <typename DataType> inline std::vector<GameObj> Layer<DataType>::all() const { return impl::wrap<GameObj>(m_impl->getIObjects()); }
template <typename DataType> inline std::vector<GameObj> Layer<DataType>::find(const Box& box) const { return impl::wrap<GameObj>(m_impl->findByBox(impl::wrap(box))); }
template <typename DataType> inline std::vector<GameObj> Layer<DataType>::find(const Vec2& v) const { return find(Box(v)); }
template <typename DataType> inline std::vector<GameObj> Layer<DataType>::find(float x, float y) const { return find(Box(x, y)); }
template <typename DataType> inline std::vector<GameObj> Layer<DataType>::find(const GameObj& obj) const { return find(obj.box()); }
template <typename DataType> inline GameObj Layer<DataType>::child(const std::string& name) const { return impl::findAndWrap<GameObj>(m_impl.get(), name); }
template <typename DataType> inline DataType& Layer<DataType>::data(const GameObj& obj) { return m_impl->data<DataType>(obj.id()); }
template <typename DataType> inline DataType& Layer<DataType>::data(int id) { return m_impl->data<DataType>(id); }
template <typename DataType> inline int Layer<DataType>::id() const { return m_impl->id(); }
template <typename DataType> inline std::string Layer<DataType>::name() const { return m_impl->name(); }
template <typename DataType> inline bool Layer<DataType>::has(int id) const { return m_impl->hasObject(id); }
template <typename DataType> inline void Layer<DataType>::remove(int id) { m_impl->removeObject(id); }
template <typename DataType> inline void Layer<DataType>::clear() { m_impl->clear(); }
template <typename DataType> inline void Layer<DataType>::update() { m_impl->update(); }
template <typename DataType> inline size_t Layer<DataType>::size() const { return m_impl->size(); }
template <typename DataType> inline bool Layer<DataType>::empty() const { return size() == 0; }
template <typename DataType> inline bool Layer<DataType>::isVisible() const { return m_impl->isVisible(); }
template <typename DataType> inline void Layer<DataType>::setVisible(bool value) { m_impl->setVisible(value); }
template <typename DataType> inline void Layer<DataType>::show() { m_impl->setVisible(true); }
template <typename DataType> inline void Layer<DataType>::hide() { m_impl->setVisible(false); }
template <typename DataType> inline Layer<DataType>::operator bool() const { return m_impl; }
template <typename DataType> template <typename T> inline T Layer<DataType>::load(const std::string& fileName)
{
	auto objImpl = impl::deserialize<impl::IObject>(fileName);
	m_impl->addObject(objImpl);
	return impl::wrap<T>(objImpl.get());
}
template <typename DataType> template <typename T> inline T Layer<DataType>::load(const std::string& fileName, float x, float y) { return load<T>(fileName, Vec2(x, y)); }
template <typename DataType> template <typename T> inline T Layer<DataType>::load(const std::string& fileName, const Vec2& pos) { auto result = load<T>(fileName); result.setPos(pos); return result; }
template <typename DataType> template <typename T> inline T Layer<DataType>::load(int id, const std::string& fileName)
{
	auto objImpl = impl::deserialize<impl::IObject>(fileName);
	m_impl->insertObject(id, objImpl);
	return impl::wrap<T>(objImpl.get());
}
template <typename DataType> template <typename T> inline T Layer<DataType>::load(int id, const std::string& fileName, float x, float y) { return load<T>(id, fileName, Vec2(x, y)); }
template <typename DataType> template <typename T> inline T Layer<DataType>::load(int id, const std::string& fileName, const Vec2& pos) { auto result = load<T>(id, fileName); result.setPos(pos); return result; }
template <typename DataType> inline GameObj Layer<DataType>::load(const std::string& fileName) { return load<GameObj>(fileName); }
template <typename DataType> inline GameObj Layer<DataType>::load(const std::string& fileName, float x, float y) { return load<GameObj>(fileName, x, y); }
template <typename DataType> inline GameObj Layer<DataType>::load(const std::string& fileName, const Vec2& pos) { return load<GameObj>(fileName, pos); }
template <typename DataType> inline GameObj Layer<DataType>::load(int id, const std::string& fileName) { return load<GameObj>(id, fileName); }
template <typename DataType> inline GameObj Layer<DataType>::load(int id, const std::string& fileName, float x, float y) { return load<GameObj>(id, fileName, x, y); }
template <typename DataType> inline GameObj Layer<DataType>::load(int id, const std::string& fileName, const Vec2& pos) { return load<GameObj>(id, fileName, pos); }

}
