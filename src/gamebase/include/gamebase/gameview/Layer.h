#pragma once

#include <gamebase/gameobj/GameObj.h>
#include <gamebase/impl/gameview/ILayer.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

template <typename DataType>
class Layer {
public:
    template <typename T> int add(const T& obj);
    template <typename T> void insert(int id, const T& obj);
    template <typename T> bool has(const T& obj) const;
    template <typename T> void remove(const T& obj);
    template <typename T> T get(int id) const;
    template <typename T> std::vector<T> all() const;
    template <typename T> std::vector<T> find(const BoundingBox& box) const;
    template <typename T> T child(const std::string& name) const;
    template <typename T> DataType& data(const T& obj);

    GameObj get(int id) const;
    std::vector<GameObj> all() const;
    std::vector<GameObj> find(const BoundingBox& box) const;
    GameObj child(const std::string& name) const;
    DataType& data(const GameObj& obj);
    DataType& data(int id);

    bool has(int id) const;
    void remove(int id);
    void clear();

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    GAMEBASE_DEFINE_PIMPL(Layer, ILayer);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename DataType> template <typename T> inline int Layer<DataType>::add(const T& obj) { return m_impl->addObject(impl::unwrapShared(obj)); }
template <typename DataType> template <typename T> inline void Layer<DataType>::insert(int id, const T& obj) { m_impl->insertObject(id, impl::unwrapShared(obj)); }
template <typename DataType> template <typename T> inline bool Layer<DataType>::has(const T& obj) const { return m_impl->hasObject(impl::unwrapRaw(obj).get()); }
template <typename DataType> template <typename T> inline void Layer<DataType>::remove(const T& obj) { m_impl->removeObject(impl::unwrapRaw(obj)); }
template <typename DataType> template <typename T> inline T Layer<DataType>::get(int id) const { return impl::wrap<T>(m_impl->getIObject(id)); }
template <typename DataType> template <typename T> inline std::vector<T> Layer<DataType>::all() const { return impl::wrap<T>(m_impl->getIObjects()); }
template <typename DataType> template <typename T> inline std::vector<T> Layer<DataType>::find(const BoundingBox& box) const { return impl::wrap<T>(m_impl->findByBox(box)); }
template <typename DataType> template <typename T> inline T Layer<DataType>::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
template <typename DataType> template <typename T> inline DataType& Layer<DataType>::data(const T& obj) { return m_impl->data<DataType>(impl::unwrapRaw(obj)); }
template <typename DataType> inline GameObj Layer<DataType>::get(int id) const { return impl::wrap<GameObj>(m_impl->getIObject(id)); }
template <typename DataType> inline std::vector<GameObj> Layer<DataType>::all() const { return impl::wrap<GameObj>(m_impl->getIObjects()); }
template <typename DataType> inline std::vector<GameObj> Layer<DataType>::find(const BoundingBox& box) const { return impl::wrap<GameObj>(m_impl->findByBox(box)); }
template <typename DataType> inline GameObj Layer<DataType>::child(const std::string& name) const { return impl::findAndWrap<GameObj>(m_impl.get(), name); }
template <typename DataType> inline DataType& Layer<DataType>::data(const GameObj& obj) { return m_impl->data<DataType>(obj.id()); }
template <typename DataType> inline DataType& Layer<DataType>::data(int id) { return m_impl->data<DataType>(id); }
template <typename DataType> inline bool Layer<DataType>::has(int id) const { return m_impl->hasObject(id); }
template <typename DataType> inline void Layer<DataType>::remove(int id) { m_impl->removeObject(id); }
template <typename DataType> inline void Layer<DataType>::clear() { m_impl->clear(); }
template <typename DataType> inline bool Layer<DataType>::isVisible() const { return m_impl->isVisible(); }
template <typename DataType> inline void Layer<DataType>::setVisible(bool value) { m_impl->setVisible(value); }
template <typename DataType> inline void Layer<DataType>::show() { m_impl->setVisible(true); }
template <typename DataType> inline void Layer<DataType>::hide() { m_impl->setVisible(false); }

}
