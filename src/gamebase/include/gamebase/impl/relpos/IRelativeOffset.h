/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/geom/BoundingBox.h>

namespace gamebase { namespace impl {

class IRelativeOffset : public virtual IObject {
public:
    virtual ~IRelativeOffset() {}

    const Transform2& get() const { return m_pos; }
    
    void setBoxes(
        const BoundingBox& parentBox, const BoundingBox& thisBox)
    {
        m_pos = ShiftTransform2(count(parentBox, thisBox));
    }

    virtual Vec2 count(
        const BoundingBox& parentBox, const BoundingBox& thisBox) const = 0;

protected:
    Transform2 m_pos;
};

} }
