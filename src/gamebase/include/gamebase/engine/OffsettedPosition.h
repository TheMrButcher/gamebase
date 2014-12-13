#pragma once

#include <gamebase/engine/IPositionable.h>

namespace gamebase {

class OffsettedPosition : public IPositionable {
public:
    OffsettedPosition(const Vec2& position = Vec2())
        : m_pos(ShiftTransform2(position))
    {}

    void setPosition(const Vec2& position)
    {
        m_pos = ShiftTransform2(position);
    }

    virtual Transform2 position() const override { return m_pos; }

    virtual Transform2 transform() const override { return m_pos; }

protected:
    Transform2 m_pos;
};

}
