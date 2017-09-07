/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/geom/IGeometry.h>
#include <gamebase/impl/geom/BoundingBox.h>

namespace gamebase { namespace impl {

class GAMEBASE_API RectGeometry : public IGeometry {
public:
    RectGeometry(const BoundingBox& box)
        : m_box(box)
    {}

    const BoundingBox& rect() const { return m_box; }

    virtual bool intersects(const IGeometry* other,
        const Transform2& thisTrans, const Transform2& otherTrans) const override
    {
        return other->intersects(this, otherTrans, thisTrans);
    }

    virtual bool intersects(const PointGeometry* other,
        const Transform2& thisTrans, const Transform2& otherTrans) const override;

    virtual bool intersects(const RectGeometry* other,
        const Transform2& thisTrans, const Transform2& otherTrans) const override;

private:
    BoundingBox m_box;
};

} }
