#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/geom/IGeometry.h>
#include <gamebase/geom/BoundingBox.h>

namespace gamebase {

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

}