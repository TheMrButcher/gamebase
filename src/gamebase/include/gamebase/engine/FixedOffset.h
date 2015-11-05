#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IRelativeOffset.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API FixedOffset : public IRelativeOffset, public ISerializable {
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

    void set(const Vec2& offset) { update(offset); }

    virtual Vec2 count(const BoundingBox&, const BoundingBox&) const override
    {
        return m_value;
    }

    virtual void serialize(Serializer& s) const override;

private:
    Vec2 m_value;
};

}
