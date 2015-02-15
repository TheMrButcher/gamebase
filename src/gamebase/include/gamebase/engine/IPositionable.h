#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/math/Transform2.h>

namespace gamebase {

class IPositionable : public virtual IObject {
public:
    IPositionable(const IPositionable* parent = nullptr)
        : m_parentPosition(parent)
    {}

    virtual Transform2 position() const = 0;

    virtual Transform2 fullTransform() const
    {
        return m_parentPosition
            ? position() * m_parentPosition->fullTransform()
            : position();
    }

    virtual void setParentPosition(const IPositionable* parent)
    {
        m_parentPosition = parent;
    }

protected:
    const IPositionable* m_parentPosition;
};

}
