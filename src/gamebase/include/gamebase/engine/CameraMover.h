#pragma once

#include <gamebase/engine/Camera.h>
#include <set>

namespace gamebase {

class GAMEBASE_API CameraMover {
public:
    CameraMover(Camera* camera = nullptr, bool callRedisplay = false, bool logging = true);

    void setCamera(Camera* camera);
    void setAngleStep(float step);
    void setScaleStep(float step);
    void setStep(float step);

    void move();

    void processKey(char key);
    void processKeyUp(char key);
    void processSpecialKey(int key);
    void processSpecialKeyUp(int key);

private:
    Camera* m_camera;
    bool m_callRedisplay;
    bool m_logging;
    float m_angleStep;
    float m_scaleStep;
    float m_step;

    std::set<char> m_pressedKeys;
    std::set<int> m_pressedSpecialKeys;
};

}
