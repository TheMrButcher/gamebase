/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/geom/IGeometry.h>
#include <gamebase/impl/geom/BoundingBox.h>
#include <gamebase/impl/engine/IObject.h>
#include <memory>

namespace gamebase { namespace impl {

class IRelativeGeometry : public virtual IObject {
public:
    virtual ~IRelativeGeometry() {}

    std::shared_ptr<IGeometry> get() const
    {
        if (!m_geom)
            THROW_EX() << "Relative geometry is not inited";
        return m_geom;
    }
    
    void setBox(const BoundingBox& box)
    {
        m_geom = count(box);
    }

    virtual std::shared_ptr<IGeometry> count(const BoundingBox& box) const = 0;

private:
    std::shared_ptr<IGeometry> m_geom;
};

} }
