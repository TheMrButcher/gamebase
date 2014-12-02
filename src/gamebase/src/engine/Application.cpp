#include <stdafx.h>
#include <gamebase/engine/Application.h>
#include <gamebase/engine/IDrawable.h>
#include <gamebase/engine/IMovable.h>
#include <gamebase/graphics/Init.h>
#include <iostream>

namespace gamebase {
namespace {
Application* app;

void displayFunc()
{
    app->displayFunc();
}

void keyboardFunc(unsigned char key, int x, int y)
{
    app->keyboardFunc(key, x, y);
}

void keyboardUpFunc(unsigned char key, int x, int y)
{
    app->keyboardUpFunc(key, x, y);
}

void specialFunc(int key, int x, int y)
{
    app->specialFunc(key, x, y);
}

void specialUpFunc(int key, int x, int y)
{
    app->specialUpFunc(key, x, y);
}

void motionFunc(int x, int y)
{
    app->motionFunc(x, y);
}

void mouseFunc(int button, int state, int x, int y)
{
    app->mouseFunc(button, state, x, y);
}
}

Application::Application()
    : m_inited(false)
    , m_moveTime(1.0f / 60)
{}

void Application::setWindowName(const std::string& name)
{
    if (m_inited) {
        std::cerr << "Warning: can't set window name, already inited" << std::endl;
        return;
    }
    m_name = name;
}

bool Application::init(int* argc, char** argv, Mode mode, int width, int height)
{
    if (m_inited) {
        std::cerr << "Warning: can't init application, already inited" << std::endl;
        return false;
    }

    try {
        m_mode = mode;
        if (m_name.empty())
            m_name = "Gamebase Application";
        if (mode == Window)
            initWindowMode(argc, argv, width, height, m_name);
        else
            initGameMode(argc, argv, width, height);
        m_fpsCounter.reset(new Counter("Frames per 10 seconds", 10.0));

        app = this;

        load();

        if (auto* loadable = dynamic_cast<IDrawable*>(m_rootObject.get()))
            loadable->loadResources();

        glutDisplayFunc(&gamebase::displayFunc);
        glutKeyboardFunc(&gamebase::keyboardFunc);
        glutKeyboardUpFunc(&gamebase::keyboardUpFunc);
        glutSpecialFunc(&gamebase::specialFunc);
        glutSpecialUpFunc(&gamebase::specialUpFunc);
        glutMotionFunc(&gamebase::motionFunc);
        glutPassiveMotionFunc(&gamebase::motionFunc);
        glutMouseFunc(&gamebase::mouseFunc);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_inited = true;
    } catch (std::exception& ex)
    {
        std::cerr << "Error while loading. Reason: " << ex.what() << std::endl;
        return false;
    }
    return true;
}

void Application::setMode(Mode mode)
{
    // ToDo
}

void Application::setScreenSize(int width, int height)
{
    // ToDo
}

void Application::setMoveTime(float time)
{
    m_moveTime = time;
}

void Application::run()
{
    glutMainLoop();
}

void Application::stop()
{
    glutLeaveGameMode();
    glutLeaveMainLoop();
}

void Application::displayFunc()
{
    if (m_fpsCounter)
        m_fpsCounter->touch();

    if (auto* movable = dynamic_cast<IMovable*>(m_rootObject.get()))
        movable->move(m_moveTime);
    move();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    try {
        if (auto* drawable = dynamic_cast<IDrawable*>(m_rootObject.get()))
            drawable->draw(projectionTransform());
        render();
    } catch (std::exception& ex)
    {
        std::cerr << "Error while rendering. Reason: " << ex.what() << std::endl;
    }

    m_inputProcessor.step();
    glutSwapBuffers();
    glutPostRedisplay();
}

void Application::keyboardFunc(unsigned char key, int, int)
{
    m_inputProcessor.keys.setDown(key);
    processKeyDown(key);
}

void Application::keyboardUpFunc(unsigned char key, int, int)
{
    m_inputProcessor.keys.setUp(key);
    processKeyUp(key);
}

void Application::specialFunc(int key, int, int)
{
    m_inputProcessor.specialKeys.setDown(key);
    processSpecialKeyDown(key);
}

void Application::specialUpFunc(int key, int, int)
{
    m_inputProcessor.specialKeys.setUp(key);
    processSpecialKeyUp(key);
}

void Application::motionFunc(int x, int y)
{
    auto normalizedMousePosition = mouseCoordsToNormalized(x, y);
    m_inputProcessor.setMousePosition(normalizedMousePosition);
    processMouseMotion(normalizedMousePosition);
}

void Application::mouseFunc(int button, int state, int x, int y)
{
    m_inputProcessor.setMousePosition(x, y);
    if (state == GLUT_DOWN) {
        m_inputProcessor.mouseButtons.setDown(button);
        processMouseButtonDown(button);
    } else {
        m_inputProcessor.mouseButtons.setUp(button);
        processMouseButtonUp(button);
    }
}

}
