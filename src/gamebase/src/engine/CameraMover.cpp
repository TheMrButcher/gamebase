#include <stdafx.h>
#include <gamebase/engine/CameraMover.h>
#include <iostream>

namespace gamebase {

CameraMover::CameraMover(Camera* camera, bool callRedisplay, bool logging)
    : m_camera(camera)
    , m_callRedisplay(callRedisplay)
    , m_logging(logging)
    , m_angleStep(0.02f)
    , m_scaleStep(0.01f)
    , m_step(5.0f)
{}

void CameraMover::setCamera(Camera* camera)
{
    m_camera = camera;
}

void CameraMover::setAngleStep(float step)
{
    m_angleStep = step;
}

void CameraMover::setScaleStep(float step)
{
    m_scaleStep = step;
}

void CameraMover::setStep(float step)
{
    m_step = step;
}

void CameraMover::move()
{
    if (!m_camera)
        return;

    bool needRedisplay = false;

    for (auto it = m_pressedKeys.begin(); it != m_pressedKeys.end(); ++it) {
        char key = *it;
        switch (key) {
            case 'a':
                m_camera->move(Vec2(-m_step, 0));
                if (m_logging) std::cout << "Key: a, new center.x=" << m_camera->center().x << std::endl;
                needRedisplay = true;
                break;
            case 'd':
                m_camera->move(Vec2(+m_step, 0));
                if (m_logging) std::cout << "Key: d, new center.x=" << m_camera->center().x << std::endl;
                needRedisplay = true;
                break;
            case 's':
                m_camera->move(Vec2(0, -m_step));
                if (m_logging) std::cout << "Key: s, new center.y=" << m_camera->center().y << std::endl;
                needRedisplay = true;
                break;
            case 'w':
                m_camera->move(Vec2(0, +m_step));
                if (m_logging) std::cout << "Key: w, new center.y=" << m_camera->center().y << std::endl;
                needRedisplay = true;
                break;
            default:
                break;
        }
    }

    for (auto it = m_pressedSpecialKeys.begin(); it != m_pressedSpecialKeys.end(); ++it) {
        int key = *it;
        switch (key) {
            case GLUT_KEY_LEFT:
                m_camera->rotate(+m_angleStep);
                if (m_logging) std::cout << "Key: left, new angle=" << m_camera->angle() << std::endl;
                needRedisplay = true;
                break;
            case GLUT_KEY_RIGHT:
                m_camera->rotate(-m_angleStep);
                if (m_logging) std::cout << "Key: right, new angle=" << m_camera->angle() << std::endl;
                needRedisplay = true;
                break;
            case GLUT_KEY_DOWN:
                m_camera->addScale(-m_scaleStep);
                if (m_logging) std::cout << "Key: down, new scale=" << m_camera->scale() << std::endl;
                needRedisplay = true;
                break;
            case GLUT_KEY_UP:
                m_camera->addScale(+m_scaleStep);
                if (m_logging) std::cout << "Key: up, new scale=" << m_camera->scale() << std::endl;
                needRedisplay = true;
                break;
            default:
                break;
        }
    }

    if (m_callRedisplay && needRedisplay)
        glutPostRedisplay();
}
void CameraMover::processKey(char key)
{
    m_pressedKeys.insert(key);
}

void CameraMover::processKeyUp(char key)
{
    m_pressedKeys.erase(key);
}

void CameraMover::processSpecialKey(int key)
{
    m_pressedSpecialKeys.insert(key);
}

void CameraMover::processSpecialKeyUp(int key)
{
    m_pressedSpecialKeys.erase(key);
}

}
