/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/findable/FindableGeometry.h>
#include <gamebase/impl/geom/PointGeometry.h>

namespace gamebase { namespace impl {

FindableGeometry::FindableGeometry(
    const IObject* position,
    const std::shared_ptr<IRelativeGeometry>& geom)
    : m_findableObj(position)
    , m_findableGeom(geom)
    , m_findableGeometryInited(false)
    , m_findablePos(nullptr)
    , m_findableDrawable(nullptr)
{}

bool FindableGeometry::isSelectableByPoint(const Vec2& point) const
{
    if (!m_findableGeometryInited) {
        m_findablePos = dynamic_cast<const IPositionable*>(m_findableObj);
        m_findableDrawable = dynamic_cast<const IDrawable*>(m_findableObj);
        m_findableGeometryInited = true;
    }

    if (m_findableDrawable && !m_findableDrawable->isVisible())
        return false;
    PointGeometry pointGeom(point);
    Transform2 thisTrans = m_findablePos ? m_findablePos->position() : Transform2();
    return m_findableGeom->get()->intersects(&pointGeom, thisTrans, Transform2());
}

} }
