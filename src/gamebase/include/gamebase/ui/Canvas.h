#pragma once

#include <gamebase/impl/ui/CanvasLayout.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Canvas {
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

    void setSizes(float width, float height);

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    Vec2 pos() const;
    void setPos(const Vec2& v);
    BoundingBox box() const;
    BoundingBox movedBox() const;

    GAMEBASE_DEFINE_PIMPL(Canvas, CanvasLayout);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline int Canvas::add(const T& obj) { m_impl->addObject(impl::unwrapShared(obj)); }
template <typename T> inline void Canvas::insert(int id, const T& obj) { m_impl->insertObject(id, impl::unwrapShared(obj)); }
template <typename T> inline bool Canvas::has(const T& obj) const { return m_impl->hasObject(impl::unwrapRaw(obj).get()); }
template <typename T> inline void Canvas::remove(const T& obj) { m_impl->removeObject(impl::unwrapRaw(obj)); }
template <typename T> inline T Canvas::get(int id) const { return impl::wrap(m_impl->getIObject(id)); }
template <typename T> inline std::vector<T> Layout::all() const { return impl::wrap(m_impl->objectsAsList()); }
template <typename T> inline T Canvas::child(const std::string& name) const { return impl::findAndWrap(m_impl.get(), name); }
inline bool Canvas::has(int id) const { return m_impl->hasObject(id); }
inline void Canvas::remove(int id) { m_impl->removeObject(id); }
inline void Canvas::clear() { m_impl->clear(); }
inline void Canvas::update() { m_impl->update(); }
inline void Canvas::setSizes(float width, float height) { m_impl->setFixedBox(width, height); }
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Canvas);

}
