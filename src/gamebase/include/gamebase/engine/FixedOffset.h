#pragma once

#include <gamebase/engine/IRelativeOffset.h>

namespace gamebase {

class FixedOffset : public IRelativeOffset {
public:
    FixedOffset() {}

    FixedOffset(float x, float y)
        : m_value(x, y)
    {}

    FixedOffset(const Vec2& offset)
        : m_value(offset)
    {}
    
    void update(const Vec2& offset)
    {
        m_value = offset;
        m_pos = ShiftTransform2(offset);
    }

    virtual Vec2 count(const BoundingBox&, const BoundingBox&) const override
    {
        return m_value;
    }

private:
    Vec2 m_value;
};

}
