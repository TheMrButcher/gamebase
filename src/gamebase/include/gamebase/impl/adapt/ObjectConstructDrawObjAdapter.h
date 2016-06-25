#pragma once

#include <gamebase/impl/adapt/IDrawObjAdapter.h>
#include <gamebase/impl/gameobj/InactiveObjectConstruct.h>

namespace gamebase { namespace impl {

class ObjectConstructDrawObjAdapter : public IDrawObjAdapter {
public:
    ObjectConstructDrawObjAdapter(const SmartPointer<InactiveObjectConstruct>& obj)
        : m_obj(obj)
    {}

    virtual bool isVisible() const override { return m_obj->isVisible(); }
    virtual void setVisible(bool value) override { m_obj->setVisible(value); }
    virtual Vec2 getOffset() const override { return m_obj->getOffset(); }
    virtual void setOffset(const Vec2& v) override { m_obj->setOffset(v); }
    virtual BoundingBox box() const override { return m_obj->box(); }
    virtual BoundingBox movedBox() const override { return m_obj->movedBox(); }
    virtual SmartPointer<IObject> getInternalObj() const override { return m_obj; }

private:
    SmartPointer<InactiveObjectConstruct> m_obj;
};

} }
