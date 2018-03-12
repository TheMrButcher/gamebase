/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/geom/IGeometry.h>

namespace gamebase { namespace impl {

class GAMEBASE_API PointGeometry : public IGeometry {
public:
    PointGeometry(const Vec2& vec)
        : m_vec(vec)
    {}

    const Vec2& position() const { return m_vec; }

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
    Vec2 m_vec;
};

} }
