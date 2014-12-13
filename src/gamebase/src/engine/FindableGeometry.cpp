#include <stdafx.h>
#include <gamebase/engine/FindableGeometry.h>
#include <gamebase/geom/PointGeometry.h>

namespace gamebase {

FindableGeometry::FindableGeometry(
    IPositionable* position,
    const std::shared_ptr<IGeometry>& geom)
    : m_associatedFindableObj(position ? static_cast<IObject*>(position) : this)
    , m_findableGeom(geom)
    , m_findablePos(position)
{}

FindableGeometry::FindableGeometry(
    IObject* associatedObj,
    const IPositionable* position,
    const std::shared_ptr<IGeometry>& geom)
    : m_associatedFindableObj(associatedObj)
    , m_findableGeom(geom)
    , m_findablePos(position)
{}

IObject* FindableGeometry::find(
    const Vec2& point, const Transform2& globalPosition)
{
    PointGeometry pointGeom(point);
    Transform2 thisTrans = m_findablePos ? m_findablePos->transform() : Transform2();
    thisTrans *= globalPosition;
    if (m_findableGeom->intersects(&pointGeom, thisTrans, Transform2()))
        return m_associatedFindableObj;
    return nullptr;
}

}
