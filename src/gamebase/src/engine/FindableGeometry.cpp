#include <stdafx.h>
#include <gamebase/engine/FindableGeometry.h>
#include <gamebase/geom/PointGeometry.h>

namespace gamebase {

FindableGeometry::FindableGeometry(
    IObject* position,
    const std::shared_ptr<IRelativeGeometry>& geom)
    : m_associatedFindableObj(position ? position : this)
    , m_findableObj(position)
    , m_findableGeom(geom)
    , m_inited(false)
    , m_findablePos(nullptr)
    , m_findableDrawable(nullptr)
{}

FindableGeometry::FindableGeometry(
    IObject* associatedObj,
    const IObject* position,
    const std::shared_ptr<IRelativeGeometry>& geom)
    : m_associatedFindableObj(associatedObj)
    , m_findableObj(position)
    , m_findableGeom(geom)
    , m_inited(false)
    , m_findablePos(nullptr)
    , m_findableDrawable(nullptr)
{}

IObject* FindableGeometry::find(
    const Vec2& point, const Transform2& globalPosition)
{
    if (!m_inited) {
        m_findablePos = dynamic_cast<const IPositionable*>(m_findableObj);
        m_findableDrawable = dynamic_cast<const IDrawable*>(m_findableObj);
        m_inited = true;
    }

    if (m_findableDrawable && !m_findableDrawable->isVisible())
        return nullptr;
    PointGeometry pointGeom(point);
    Transform2 thisTrans = m_findablePos ? m_findablePos->position() : Transform2();
    thisTrans *= globalPosition;
    if (m_findableGeom->get()->intersects(&pointGeom, thisTrans, Transform2()))
        return m_associatedFindableObj;
    return nullptr;
}

}
