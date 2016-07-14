/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/geom/PointGeometry.h>
#include <gamebase/impl/geom/RectGeometry.h>
#include <gamebase/math/Math.h>

namespace gamebase { namespace impl {

bool PointGeometry::intersects(const PointGeometry* other,
    const Transform2& thisTrans, const Transform2& otherTrans) const
{
    return isEqual((thisTrans * m_vec - otherTrans * other->m_vec).length(), 0);
}

bool PointGeometry::intersects(const RectGeometry* other,
    const Transform2& thisTrans, const Transform2& otherTrans) const
{
    return other->intersects(this, otherTrans, thisTrans);
}

} }
