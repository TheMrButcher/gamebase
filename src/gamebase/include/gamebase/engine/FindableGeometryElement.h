#pragma once

#include <gamebase/engine/FindableElement.h>
#include <gamebase/engine/FindableGeometry.h>

namespace gamebase {

class GAMEBASE_API FindableGeometryElement : public FindableElement, public FindableGeometry {
public:
    FindableGeometryElement(const std::shared_ptr<IRelativeGeometry>& geom);

    virtual void connectWith(const IObject* obj) override
    {
        m_findableGeometryInited = false;
        m_findableObj = obj;
    }

    virtual void setBox(const BoundingBox& box) override
    {
        m_findableGeom->setBox(box);
    }

    virtual void registerObject(PropertiesRegisterBuilder*) override {}
    virtual void serialize(Serializer& serializer) const override;
};

}
