/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

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

    virtual bool hasObject(IObject* obj) const override
    {
        const auto& objs = objects();
        for (auto it = objs.begin(); it != objs.end(); ++it)
            if (it->get() == obj)
                return true;
        return false;
    }

    virtual IObject* tryGetAbstractObject(const std::string& name) const override { return m_registrable->tryGetAbstractChild(name); }
    virtual IObject* getAbstractChild(const std::string& name) const override { return m_registrable->getAbstractChild(name); }

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
