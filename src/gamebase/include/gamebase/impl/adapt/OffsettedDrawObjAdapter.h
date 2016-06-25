#pragma once

#include <gamebase/impl/adapt/IDrawObjAdapter.h>
#include <gamebase/impl/engine/IDrawable.h>
#include <gamebase/impl/pos/OffsettedPosition.h>

namespace gamebase { namespace impl {

class OffsettedDrawObjAdapter : public IDrawObjAdapter {
public:
    OffsettedDrawObjAdapter(
        const SmartPointer<OffsettedPosition>& pos,
        IDrawable* drawable)
        : m_pos(pos)
        , m_drawable(drawable)
    {}

    virtual bool isVisible() const override { return m_drawable->isVisible(); }
    virtual void setVisible(bool value) override { m_drawable->setVisible(value); }
    virtual Vec2 getOffset() const override { return m_pos->getOffset(); }
    virtual void setOffset(const Vec2& v) override { m_pos->setOffset(v); }
    virtual BoundingBox box() const override { return m_drawable->box(); }
    virtual BoundingBox movedBox() const override { return m_drawable->box().moved(m_pos->getOffset()); }
    virtual SmartPointer<IObject> getInternalObj() const override { return m_pos; }

private:
    SmartPointer<OffsettedPosition> m_pos;
    IDrawable* m_drawable;
};

} }
