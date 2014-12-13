#include <stdafx.h>
#include <gamebase/geom/PointGeometry.h>
#include <gamebase/geom/RectGeometry.h>

namespace gamebase {

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

}
