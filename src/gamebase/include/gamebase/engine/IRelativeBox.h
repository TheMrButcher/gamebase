#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/geom/BoundingBox.h>
#include <gamebase/utils/Exception.h>

namespace gamebase {

class IRelativeBox : public virtual IObject {
public:
    IRelativeBox() : m_inited(false)  {}

    virtual ~IRelativeBox() {}

    bool isValid() const { return m_inited && m_box.isValid(); }

    const BoundingBox& get() const
    {
        if (!isValid())
            THROW_EX() << "IRelativeBox is not inited";
        return m_box;
    }
    
    void setParentBox(const BoundingBox& parentBox)
    {
        m_inited = true;
        m_box = count(parentBox);
    }

    virtual BoundingBox count(const BoundingBox& parentBox) const = 0;

protected:
    bool m_inited;
    BoundingBox m_box;
};

}
