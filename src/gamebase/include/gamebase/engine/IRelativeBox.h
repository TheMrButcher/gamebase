#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/geom/BoundingBox.h>

namespace gamebase {

class IRelativeBox : public virtual IObject {
public:
    virtual ~IRelativeBox() {}

    const BoundingBox& get() const { return m_box; }
    
    void setParentBox(const BoundingBox& parentBox)
    {
        m_box = count(parentBox);
    }

    virtual BoundingBox count(const BoundingBox& parentBox) const = 0;

protected:
    BoundingBox m_box;
};

}
