#pragma once

#include <gamebase/impl/pos/IPositionable.h>

namespace gamebase { namespace impl {

class TransformedPosition : virtual public IPositionable {
public:
    TransformedPosition()
        : m_scaleX(1)
        , m_scaleY(1)
        , m_angle(0)
    {}

    TransformedPosition(float scaleX, float scaleY, float angle, const Vec2& offset = Vec2())
        : m_pos(RotationMatrix2(angle) * ScalingMatrix2(scaleX, scaleY), offset)
        , m_scaleX(scaleX)
        , m_scaleY(scaleY)
        , m_angle(angle)
    {}

    Vec2 getOffset() const { return position().offset; }
    void setOffset(const Vec2& v) { m_pos.offset = v; }

    float scale() const { return m_scaleX; }
    void setScale(float scale)
    {
        m_scaleX = scale;
        m_scaleY = scale;
        updateMatrix();
    }

    float scaleX() const { return m_scaleX; }
    void setScaleX(float scale)
    {
        if (m_scaleX == scale)
            return;
        m_scaleX = scale;
        updateMatrix();
    }

    float scaleY() const { return m_scaleY; }
    void setScaleY(float scale)
    {
        if (m_scaleY == scale)
            return;
        m_scaleY = scale;
        updateMatrix();
    }

    float angle() const { return m_angle; }
    void setAngle(float angle)
    {
        if (m_angle == angle)
            return;
        m_angle = angle;
        updateMatrix();
    }

    virtual Transform2 position() const override
    {
        return m_pos;
    }

protected:
    void updateMatrix()
    {
        if (m_angle == 0) {
            if (m_scaleX == 1 && m_scaleY == 1)
                m_pos.matrix = Matrix2();
            else
                m_pos.matrix = ScalingMatrix2(m_scaleX, m_scaleY);
        } else {
            if (m_scaleX == 1 && m_scaleY == 1)
                m_pos.matrix = RotationMatrix2(m_angle);
            else
                m_pos.matrix = RotationMatrix2(m_angle) * ScalingMatrix2(m_scaleX, m_scaleY);
        }
    }

    Transform2 m_pos;
    float m_scaleX;
    float m_scaleY;
    float m_angle;
};

} }
