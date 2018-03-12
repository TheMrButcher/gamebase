/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/geom/PointGeometry.h>
#include <gamebase/impl/geom/RectGeometry.h>

namespace gamebase { namespace impl {

bool RectGeometry::intersects(const PointGeometry* other,
    const Transform2& thisTrans, const Transform2& otherTrans) const
{
    return m_box.contains(otherTrans * thisTrans.inversed() * other->position());
}

bool RectGeometry::intersects(const RectGeometry* other,
    const Transform2& thisTrans, const Transform2& otherTrans) const
{
    // ToDo
    return false;
}

} }
