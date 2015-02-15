#include <stdafx.h>
#include <gamebase/engine/RotatingObject.h>
#include <boost/math/constants/constants.hpp>

namespace gamebase {

void RotatingObject::setTrajectoryPeriod(float period)
{
    m_trajectoryAngleSpeed = 2.0f * boost::math::constants::pi<float>() / period;
}

void RotatingObject::setRotationPeriod(float period)
{
    m_rotationAngleSpeed = 2.0f * boost::math::constants::pi<float>() / period;
}

Transform2 RotatingObject::position() const
{
    Vec2 position = RotationMatrix2(m_trajectoryAngle) * Vec2(m_trajectoryRadius, 0);
    return ShiftTransform2(position);
}

Transform2 RotatingObject::fullTransform() const
{
    return RotationTransform2(m_rotationAngle)
        * position();
}

void RotatingObject::move(float time)
{
    m_trajectoryAngle += m_trajectoryAngleSpeed * time;
    m_rotationAngle += m_rotationAngleSpeed * time;
}

}
