#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/relpos/IRelativeOffset.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

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

    const Vec2& get() const { return m_value; }

    void set(const Vec2& offset) { update(offset); }

    virtual Vec2 count(const BoundingBox&, const BoundingBox&) const override
    {
        return m_value;
    }

    virtual void serialize(Serializer& s) const override;

private:
    Vec2 m_value;
};

} }
