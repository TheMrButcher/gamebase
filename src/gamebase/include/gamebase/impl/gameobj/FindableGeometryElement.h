/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameobj/FindableElement.h>
#include <gamebase/impl/findable/FindableGeometry.h>

namespace gamebase { namespace impl {

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

} }
