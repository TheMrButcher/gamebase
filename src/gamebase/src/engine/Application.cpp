#include <stdafx.h>
#include <gamebase/engine/Application.h>
#include "SpecialKeyConverter.h"
#include <gamebase/engine/IDrawable.h>
#include <gamebase/engine/IMovable.h>
#include <gamebase/engine/IInputProcessor.h>
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

        m_mouseOnObject = nullptr;
        m_selectedObject = nullptr;
        m_associatedSelectable = nullptr;

        app = this;

        load();

        m_rootObject.setBox(BoundingBox(
            Vec2(-0.5f * width, -0.5f * height),
            Vec2(0.5f * width, 0.5f * height)));
        m_rootObject.loadResources();

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
    } catch (std::exception& ex) {
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

    processMouseActions();
    if (auto keyProcessor = dynamic_cast<IInputProcessor*>(m_selectedObject))
        keyProcessor->processInput(m_inputRegister);
    processInput();

    m_rootObject.move(m_moveTime);
    move();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    try {
        m_rootObject.draw(projectionTransform());
        render();
    } catch (std::exception& ex)
    {
        std::cerr << "Error while rendering. Reason: " << ex.what() << std::endl;
    }

    m_inputRegister.step();
    glutSwapBuffers();
    glutPostRedisplay();
}

void Application::keyboardFunc(unsigned char key, int, int)
{
    m_inputRegister.keys.setDown(key);
    processKeyDown(key);
}

void Application::keyboardUpFunc(unsigned char key, int, int)
{
    m_inputRegister.keys.setUp(key);
    processKeyUp(key);
}

void Application::specialFunc(int key, int, int)
{
    m_inputRegister.specialKeys.setDown(convertGLUTCode(key));
    processSpecialKeyDown(key);
}

void Application::specialUpFunc(int key, int, int)
{
    m_inputRegister.specialKeys.setUp(convertGLUTCode(key));
    processSpecialKeyUp(key);
}

void Application::motionFunc(int x, int y)
{
    auto convertedMousePosition = convertMouseCoords(x, y);
    m_inputRegister.setMousePosition(convertedMousePosition);
    processMouseMotion(convertedMousePosition);
}

void Application::mouseFunc(int buttonCode, int state, int x, int y)
{
    m_inputRegister.setMousePosition(x, y);
    MouseButton::Enum button;
    switch (buttonCode) {
        case GLUT_LEFT_BUTTON:   button = MouseButton::Left; break;
        case GLUT_MIDDLE_BUTTON: button = MouseButton::Middle; break;
        case GLUT_RIGHT_BUTTON:  button = MouseButton::Right; break;
        default: return;
    }
    if (state == GLUT_DOWN) {
        m_inputRegister.mouseButtons.setDown(button);
        processMouseButtonDown(button);
    } else {
        m_inputRegister.mouseButtons.setUp(button);
        processMouseButtonUp(button);
    }
}

namespace {
IObject* filterDisabled(IObject* obj)
{
    if (auto selectable = dynamic_cast<ISelectable*>(obj)) {
        if (selectable->selectionState() == SelectionState::Disabled)
           return nullptr;
    }
    return obj;
}
}

void Application::processMouseActions()
{
    IObject* curObject = m_rootObject.find(
        m_inputRegister.mousePosition(), Transform2());
    curObject = filterDisabled(curObject);
    m_mouseOnObject = filterDisabled(m_mouseOnObject);
    m_selectedObject = filterDisabled(m_selectedObject);
    m_associatedSelectable = filterDisabled(m_associatedSelectable);

    if (m_inputRegister.mouseButtons.isPressed(MouseButton::Left)) {
        if (m_inputRegister.mouseButtons.isJustPressed(MouseButton::Left)) {
            if (m_selectedObject && m_selectedObject != curObject) {
                if (auto selectable = dynamic_cast<ISelectable*>(curObject)) {
                    if (m_associatedSelectable) {
                        if (selectable->associatedSelectable() != m_associatedSelectable) {
                            if (curObject != m_associatedSelectable) {
                                dynamic_cast<ISelectable*>(m_associatedSelectable)->setSelectionState(
                                    SelectionState::None);
                            }
                            m_associatedSelectable = nullptr;
                        }
                    }
                    if (selectable->associatedSelectable() == m_selectedObject) {
                        m_associatedSelectable = m_selectedObject;
                        m_selectedObject = nullptr;
                    }
                }
                if (auto selectable = dynamic_cast<ISelectable*>(m_selectedObject)) {
                    selectable->setSelectionState(SelectionState::None);
                    m_selectedObject = nullptr;
                }
            }
            if (m_mouseOnObject != curObject) {
                changeSelectionState(SelectionState::None);
                m_mouseOnObject = curObject;
            }
            if (auto selectable = dynamic_cast<ISelectable*>(m_selectedObject)) {
                if (selectable->selectionState() != SelectionState::Selected
                    && selectable->selectionState() != SelectionState::Pressed)
                    m_selectedObject = nullptr;
            }
            if (auto selectable = dynamic_cast<ISelectable*>(m_mouseOnObject)) {
                selectable->setSelectionState(SelectionState::Pressed);
                if (selectable->selectionState() == SelectionState::Selected
                    || selectable->selectionState() == SelectionState::Pressed)
                    m_selectedObject = m_mouseOnObject;
            }
        }
    } else {
        if (m_mouseOnObject != curObject) {
            changeSelectionState(SelectionState::None);
            m_mouseOnObject = curObject;
            if (auto selectable = dynamic_cast<ISelectable*>(m_mouseOnObject))
                selectable->setSelectionState(SelectionState::MouseOn);
        }
        if (m_inputRegister.mouseButtons.isJustOutpressed(MouseButton::Left)) {
            bool unselectIfPressed = true;
            if (auto selectable = dynamic_cast<ISelectable*>(m_mouseOnObject)) {
                if (selectable->selectionState() == SelectionState::Pressed) {
                    selectable->setSelectionState(SelectionState::Selected);
                    if (selectable->selectionState() == SelectionState::Selected
                        || selectable->selectionState() == SelectionState::Pressed) {
                        if (m_selectedObject && m_selectedObject != curObject)
                            dynamic_cast<ISelectable*>(m_selectedObject)->setSelectionState(
                                SelectionState::None);
                        m_selectedObject = curObject;
                        unselectIfPressed = false;
                    } else {
                        m_selectedObject = nullptr;
                    }
                }
            }
            if (auto selectable = dynamic_cast<ISelectable*>(m_selectedObject)) {
                if (unselectIfPressed && selectable->selectionState() == SelectionState::Pressed) {
                    selectable->setSelectionState(SelectionState::None);
                    m_selectedObject = nullptr;
                }
            }
        }
    }
}

void Application::changeSelectionState(SelectionState::Enum state)
{
    if (m_mouseOnObject == m_selectedObject)
        return;
    if (auto selectable = dynamic_cast<ISelectable*>(m_mouseOnObject))
        selectable->setSelectionState(state);
}

}
