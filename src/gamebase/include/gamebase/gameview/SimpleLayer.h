#pragma once

#include <gamebase/gameview/Layer.h>

namespace gamebase {

typedef Layer<void> SimpleLayer;

template <>
class Layer<void> {
public:
    template <typename T> int add(const T& obj);
    template <typename T> void insert(int id, const T& obj);
    template <typename T> bool has(const T& obj) const;
    template <typename T> void remove(const T& obj);
    template <typename T> T get(int id) const;
    template <typename T> std::vector<T> all() const;
    template <typename T> std::vector<T> find(const BoundingBox& box) const;
    template <typename T> T child(const std::string& name) const;

    GameObj get(int id) const;
    std::vector<GameObj> all() const;
    std::vector<GameObj> find(const BoundingBox& box) const;
    GameObj child(const std::string& name) const;

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

template <typename T> inline int Layer<void>::add(const T& obj) { return m_impl->addObject(impl::unwrapShared(obj)); }
template <typename T> inline void Layer<void>::insert(int id, const T& obj) { m_impl->insertObject(id, impl::unwrapShared(obj)); }
template <typename T> inline bool Layer<void>::has(const T& obj) const { return m_impl->hasObject(impl::unwrapRaw(obj).get()); }
template <typename T> inline void Layer<void>::remove(const T& obj) { m_impl->removeObject(impl::unwrapRaw(obj)); }
template <typename T> inline T Layer<void>::get(int id) const { return impl::wrap<T>(m_impl->getIObject(id)); }
template <typename T> inline std::vector<T> Layer<void>::all() const { return impl::wrap<T>(m_impl->getIObjects()); }
template <typename T> inline std::vector<T> Layer<void>::find(const BoundingBox& box) const { return impl::wrap<T>(m_impl->findByBox(box)); }
template <typename T> inline T Layer<void>::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline GameObj Layer<void>::get(int id) const { return impl::wrap<GameObj>(m_impl->getIObject(id)); }
inline std::vector<GameObj> Layer<void>::all() const { return impl::wrap<GameObj>(m_impl->getIObjects()); }
inline std::vector<GameObj> Layer<void>::find(const BoundingBox& box) const { return impl::wrap<GameObj>(m_impl->findByBox(box)); }
inline GameObj Layer<void>::child(const std::string& name) const { return impl::findAndWrap<GameObj>(m_impl.get(), name); }
inline bool Layer<void>::has(int id) const { return m_impl->hasObject(id); }
inline void Layer<void>::remove(int id) { m_impl->removeObject(id); }
inline void Layer<void>::clear() { m_impl->clear(); }
inline bool Layer<void>::isVisible() const { return m_impl->isVisible(); }
inline void Layer<void>::setVisible(bool value) { m_impl->setVisible(value); }
inline void Layer<void>::show() { m_impl->setVisible(true); }
inline void Layer<void>::hide() { m_impl->setVisible(false); }

}
