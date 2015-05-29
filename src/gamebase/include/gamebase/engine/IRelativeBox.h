#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/geom/BoundingBox.h>
#include <gamebase/utils/Exception.h>

namespace gamebase {

class IRelativeBox : public virtual IObject {
public:
    virtual ~IRelativeBox() {}

    bool isValid() const { return m_box.isValid(); }

    const BoundingBox& get() const
    {
        if (!isValid())
            THROW_EX() << "IRelativeBox is not inited";
        return m_box;
    }
    
    void setParentBox(const BoundingBox& parentBox)
    {
        m_box = count(parentBox);
    }

    virtual BoundingBox count(const BoundingBox& parentBox) const = 0;

protected:
    BoundingBox m_box;
};

}
