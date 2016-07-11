#pragma once

#include <gamebase/impl/ui/SelectingWidget.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Selector {
public:
    template <typename T> int add(const T& obj);
    template <typename T> void insert(int id, const T& obj);
    template <typename T> T child(const std::string& name) const;

    void select(int id);
    int selected() const;

    void remove(int id);
    void clear();
    void update();

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

    GAMEBASE_DEFINE_PIMPL(Selector, SelectingWidget);
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline int Selector::add(const T& obj) { m_impl->addObject(impl::unwrapShared(obj)); }
template <typename T> inline void Selector::insert(int id, const T& obj) { m_impl->insertObject(id, impl::unwrapShared(obj)); }
template <typename T> inline T Selector::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline void Selector::select(int id) { m_impl->select(id); }
inline int Selector::selected() const { return m_impl->selected(); }
inline void Selector::remove(int id) { m_impl->removeObject(id); }
inline void Selector::clear() { m_impl->clear(); }
inline void Selector::update() { m_impl->markAllNeedReload(); select(selected()); }
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Selector);

}
