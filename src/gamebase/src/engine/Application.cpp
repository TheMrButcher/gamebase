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
    : ViewController("main")
    , m_inited(false)
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
        m_focusedController = nullptr;

        app = this;
        
        // calls Application::load(), that creates all ViewControllers
        initView();

        for (auto it = m_controllers.begin(); it != m_controllers.end(); ++it)
            it->second->initView();

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
    filterControllers();

    if (m_fpsCounter)
        m_fpsCounter->touch();

    try {
        processMouseActions();
    
        if (auto keyProcessor = dynamic_cast<IInputProcessor*>(m_selectedObject))
            keyProcessor->processInput(m_inputRegister);
        for (auto it = m_activeControllers.begin(); it != m_activeControllers.end(); ++it)
            (*it)->processInput(m_inputRegister);
    } catch (std::exception& ex)
    {
        std::cerr << "Error while processing input. Reason: " << ex.what() << std::endl;
    }

    for (auto it = m_activeControllers.begin(); it != m_activeControllers.end(); ++it)
        (*it)->moveView();
    move();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    try {
        for (auto it = m_activeControllers.begin(); it != m_activeControllers.end(); ++it)
            (*it)->renderView();
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

void Application::deactivateAllControllers()
{
    for (auto it = m_activeControllers.begin(); it != m_activeControllers.end(); ++it)
        (*it)->deactivate();
    m_focusedController = nullptr;
}

void Application::deactivateControllerByName(const std::string& controllerName)
{
    if (controllerName == id())
        deactivateController(this);
    deactivateController(m_controllers.at(controllerName).get());
}

void Application::deactivateController(ViewController* controller)
{
    auto it = std::find(m_activeControllers.begin(), m_activeControllers.end(), controller);
    if (it == m_activeControllers.end()) {
        std::cerr << "Controller with ID=" << controller->id() << " is already deactivated";
        return;
    }
    controller->deactivate();
}

void Application::activateControllerByName(const std::string& controllerName)
{
    if (controllerName == id())
        activateController(this);
    activateController(m_controllers.at(controllerName).get());
}

void Application::activateController(ViewController* controller)
{
    auto it = std::find(m_activeControllers.begin(), m_activeControllers.end(), controller);
    if (it != m_activeControllers.end()) {
        if ((*it)->viewState() == ViewController::Inactive)
            (*it)->activate();
        else
            std::cerr << "Controller with ID=" << controller->id() << " is already activated";
        return;
    }
    m_activeControllers.push_back(controller);
    controller->activate();
    sortControllers();
}

void Application::registerController(const std::shared_ptr<ViewController>& controller)
{
    if (m_controllers.find(controller->id()) != m_controllers.end())
        THROW_EX() << "Already registered controller with ID=" << controller->id();
    m_controllers[controller->id()] = controller;
}

namespace {
bool compareContollers(const ViewController* con1, const ViewController* con2)
{
    return con1->zIndex() < con2->zIndex()
        || (con1->zIndex() == con2->zIndex() && con2->viewState() == ViewController::Focused);
}
}

void Application::sortControllers()
{
    std::stable_sort(m_activeControllers.begin(), m_activeControllers.end(), &compareContollers);
}

ViewController* Application::currentController()
{
    return m_focusedController ? m_focusedController : this;
}

void Application::setFocus(ViewController* controller)
{
    if (controller == m_focusedController)
        return;
    if (m_focusedController)
        m_focusedController->lostFocus();
    m_focusedController = controller;
    if (m_focusedController)
        m_focusedController->focused();
    sortControllers();
}

void Application::filterControllers()
{
    ViewController* lastActive = nullptr;
    for (auto it = m_activeControllers.begin(); it != m_activeControllers.end();) {
        auto* viewController = *it;
        if (viewController->viewState() == ViewController::Visible)
            lastActive = viewController;
        if (viewController->viewState() == ViewController::Invisible)
            it = m_activeControllers.erase(it);
        else
            ++it;
    }

    if (m_focusedController && m_focusedController->viewState() != Focused)
        m_focusedController = nullptr;
    if (!m_focusedController && lastActive)
        setFocus(lastActive);
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
    m_mouseOnObject = filterDisabled(m_mouseOnObject);
    m_selectedObject = filterDisabled(m_selectedObject);
    m_associatedSelectable = filterDisabled(m_associatedSelectable);

    auto mousePos = m_inputRegister.mousePosition();
    for (auto it = m_activeControllers.rbegin(); it != m_activeControllers.rend(); ++it) {
        auto* viewController = *it;
        auto* view = viewController->view();
        auto box = view->box().transformed(view->fullTransform());
        if (box.contains(mousePos)) {
            IObject* curObject = view->find(mousePos, Transform2());
            if (curObject != nullptr) {
                if (viewController->viewState() != ViewController::Inactive) {
                    if (m_inputRegister.mouseButtons.isJustPressed(MouseButton::Left))
                        setFocus(viewController);
                    processMouseActions(curObject);
                }
                return;
            }
        }
    }
}

void Application::processMouseActions(IObject* curObject)
{
    curObject = filterDisabled(curObject);

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
