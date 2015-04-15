#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IMovable.h>
#include <gamebase/math/Transform2.h>

namespace gamebase {

class GAMEBASE_API RotatingObject : public IMovable {
public:
    RotatingObject();

    void setTrajectoryRadius(float radius) { m_trajectoryRadius = radius; }
    void setTrajectoryAngle(float angle) { m_trajectoryAngle = angle; }
    void setTrajectoryPeriod(float period);

    void setRotationAngle(float angle) { m_rotationAngle = angle; }
    void setRotationPeriod(float period);

    virtual Transform2 position() const override;
    Transform2 fullTransform() const;
    virtual void move() override;

private:
    float m_trajectoryRadius;
    float m_trajectoryAngle;
    float m_trajectoryAngleSpeed;
    float m_rotationAngle;
    float m_rotationAngleSpeed;
};

}
