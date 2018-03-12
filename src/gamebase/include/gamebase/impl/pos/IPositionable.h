/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/math/Transform2.h>

namespace gamebase { namespace impl {

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

} }
