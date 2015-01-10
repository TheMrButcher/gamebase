#pragma once

#include <gamebase/geom/BoundingBox.h>
#include <gamebase/geom/IGeometry.h>

namespace gamebase {

class IRelativeGeometry {
public:
    virtual ~IRelativeGeometry() {}

    std::shared_ptr<IGeometry> get() const { return m_geom; }
    
    void setBox(const BoundingBox& box)
    {
        m_geom = count(box);
    }

    virtual std::shared_ptr<IGeometry> count(const BoundingBox& box) const = 0;

private:
    std::shared_ptr<IGeometry> m_geom;
};

}
