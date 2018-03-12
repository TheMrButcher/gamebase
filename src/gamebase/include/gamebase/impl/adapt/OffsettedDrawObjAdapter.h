/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/adapt/IDrawObjAdapter.h>
#include <gamebase/impl/engine/IDrawable.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/reg/IRegistrable.h>

namespace gamebase { namespace impl {

class OffsettedDrawObjAdapter : public IDrawObjAdapter {
public:
    OffsettedDrawObjAdapter(
        const SmartPointer<OffsettedPosition>& pos,
        IDrawable* drawable)
        : m_pos(pos)
        , m_drawable(drawable)
        , m_reg(nullptr)
    {}

    virtual IObject* getAbstractChild(const std::string& name) const override
    {
        if (!m_reg) {
            m_reg = dynamic_cast<IRegistrable*>(m_drawable);
            if (!m_reg)
                THROW_EX() << "Can't get child with name=" << name
                    << " of object of type=" << typeid(*m_drawable).name();
        }
        return m_reg->getAbstractChild(name);
    }

    virtual bool isVisible() const override { return m_drawable->isVisible(); }
    virtual void setVisible(bool value) override { m_drawable->setVisible(value); }
    virtual Vec2 getOffset() const override { return m_pos->getOffset(); }
    virtual void setOffset(const Vec2& v) override { m_pos->setOffset(v); }
    virtual BoundingBox box() const override { return m_drawable->box(); }
    virtual BoundingBox movedBox() const override { auto b = m_drawable->box(); b.move(m_pos->getOffset()); return b; }
    virtual BoundingBox transformedBox() const override { return movedBox(); }
    virtual SmartPointer<IObject> getInternalObj() const override { return m_pos; }

private:
    SmartPointer<OffsettedPosition> m_pos;
    IDrawable* m_drawable;
    mutable IRegistrable* m_reg;
};

} }
