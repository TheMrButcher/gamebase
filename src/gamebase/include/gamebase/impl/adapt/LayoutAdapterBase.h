#pragma once

#include <gamebase/impl/adapt/ILayoutAdapter.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/reg/IRegistrable.h>
#include <gamebase/impl/engine/Drawable.h>

namespace gamebase { namespace impl {

class LayoutAdapterBase : public ILayoutAdapter {
public:
    LayoutAdapterBase(
        OffsettedPosition* pos,
        Drawable* drawable,
        IRegistrable* registrable)
        : m_pos(pos)
        , m_drawable(drawable)
        , m_registrable(registrable)
    {}

    virtual IObject* getChild(const std::string& name) const override { return m_registrable->getChild<IObject>(name); }

    virtual bool isVisible() const override { return m_drawable->isVisible(); }
    virtual void setVisible(bool value) override { m_drawable->setVisible(value); }

    virtual Vec2 getOffset() const override { return m_pos->getOffset(); }
    virtual void setOffset(const Vec2& v) override { m_pos->setOffset(v); }
    virtual BoundingBox box() const override { return m_drawable->box(); }
    virtual BoundingBox movedBox() const override { return m_drawable->movedBox(); }

private:
    OffsettedPosition* m_pos;
    Drawable* m_drawable;
    IRegistrable* m_registrable;
};

} }
