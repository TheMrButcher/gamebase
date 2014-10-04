#pragma once

#include <gamebase/engine/ProjectionTransform.h>

namespace gamebase {

class Camera {
public:
    Camera()
        : m_angle(0)
        , m_scale(1)
        , m_center(0, 0)
    {}

    Transform2 transform() const
    {
        return ShiftTransform2(-m_center)
            * RotationTransform2(m_angle)
            * ScalingTransform2(m_scale)
            * projectionTransform();
    }

    Camera& rotate(float angle)
    {
        m_angle += angle;
        return *this;
    }

    Camera& setAngle(float angle)
    {
        m_angle = angle;
        return *this;
    }

    float angle() const { return m_angle; }

    Camera& addScale(float scale)
    {
        m_scale += scale;
        return *this;
    }

    Camera& setScale(float scale)
    {
        m_scale = scale;
        return *this;
    }

    float scale() const { return m_scale; }

    Camera& move(const Vec2& offset)
    {
        m_center += offset;
        return *this;
    }

    Camera& setCenter(const Vec2& center)
    {
        m_center = center;
        return *this;
    }

    const Vec2& center() const { return m_center; }

private:
    float m_angle;
    float m_scale;
    Vec2 m_center;
};

}
