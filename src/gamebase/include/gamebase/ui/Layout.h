#pragma once

#include <gamebase/impl/adapt/ILayoutAdapter.h>
#include <gamebase/impl/pubhelp/Wrapping.h>

namespace gamebase {

class Layout {
public:
    template <typename T>
    void add(const T& obj);

    template <typename T>
    std::vector<T> all() const;

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

public:
    typedef impl::ILayoutAdapter Impl;
    Layout(const std::shared_ptr<Impl>& impl = nullptr) : m_impl(impl) {}
    const std::shared_ptr<Impl>& getImpl() const { return m_impl; }

private:
    std::shared_ptr<Impl> m_impl;
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline void Layout::add(const T& obj) { m_impl->addObject(impl::unwrap(obj)); }
template <typename T> inline std::vector<T> Layout::all() const { return impl::wrap(m_impl->objects()); }
inline void Layout::clear() { m_impl->clear(); }
inline void Layout::update() { m_impl->update(); }
inline void Layout::setSizes(float width, float height) { m_impl->setFixedBox(width, height); }
inline bool Layout::isVisible() const { return m_impl->isVisible(); }
inline void Layout::setVisible(bool value) { m_impl->setVisible(value); }
inline void Layout::show() { m_impl->setVisible(true); }
inline void Layout::hide() { m_impl->setVisible(false); }
inline Vec2 Layout::pos() const { return m_impl->getOffset(); }
inline void Layout::setPos(const Vec2& v) { m_impl->setOffset(v); }
inline BoundingBox Layout::box() const { return m_impl->box(); }
inline BoundingBox Layout::movedBox() const { return m_impl->movedBox(); }

}
