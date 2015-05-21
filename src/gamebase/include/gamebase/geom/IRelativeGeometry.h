#pragma once

#include <gamebase/geom/BoundingBox.h>
#include <gamebase/geom/IGeometry.h>
#include <gamebase/engine/IObject.h>

namespace gamebase {

class IRelativeGeometry : public virtual IObject {
public:
    virtual ~IRelativeGeometry() {}

    std::shared_ptr<IGeometry> get() const
    {
        if (!m_geom)
            THROW_EX() << "Relative geometry is not inited";
        return m_geom;
    }
    
    void setBox(const BoundingBox& box)
    {
        m_geom = count(box);
    }

    virtual std::shared_ptr<IGeometry> count(const BoundingBox& box) const = 0;

private:
    std::shared_ptr<IGeometry> m_geom;
};

}
