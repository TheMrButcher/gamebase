#include <stdafx.h>
#include <gamebase/engine/Application.h>
#include "SpecialKeyConverter.h"
#include "src/core/Config.h"
#include "src/core/GlobalTemporary.h"
#include "src/graphics/State.h"
#include "src/graphics/InitInternal.h"
#include <gamebase/engine/IDrawable.h>
#include <gamebase/engine/IMovable.h>
#include <gamebase/engine/IInputProcessor.h>
#include <gamebase/engine/CanvasLayout.h>
#include <gamebase/engine/OffsettedBox.h>
#include <gamebase/engine/TimeState.h>
#include <gamebase/core/Core.h>
#include <gamebase/graphics/Clipping.h>
#include <iostream>

namespace gamebase {
Application* app;
TimeState TimeState::realTime_;
TimeState TimeState::gameTime_;

const std::string TOP_VIEW_CONTROLLER_ID = "app_top";

namespace {
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

void resize(int, int)
{
    app->restoreSize();
}

class RegisterRoot : public Registrable {
public:
    RegisterRoot(
        std::shared_ptr<Panel>& appView,
        std::map<std::string, std::shared_ptr<ViewController>>& views)
        : m_appView(appView)\
        , m_views(views)
    {
        m_register.setName("root");
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder)
    {
        if (m_appView) {
            if (m_appView->name().empty())
                m_appView->setName("appView");
            builder->registerObject(m_appView.get());
        }

        for (auto it = m_views.begin(); it != m_views.end(); ++it) {
            auto view = it->second->view();
            if (view) {
                if (view->name().empty())
                    view->setName(it->first + "View");
                builder->registerObject(view.get());
            }
        }
    }

private:
    std::shared_ptr<Panel>& m_appView;
    std::map<std::string, std::shared_ptr<ViewController>>& m_views;
};

class TopViewPanelSkin : public PanelSkin {
public:
    TopViewPanelSkin() {}

    virtual std::shared_ptr<Button> createCloseButton() const { return nullptr; }
    virtual std::shared_ptr<ScrollDragBar> createDragBar() const { return nullptr; }
    virtual bool isTransparent() const { return true; }
    virtual BoundingBox panelBox() const { return m_box; }
    virtual void loadResources() override {}
    virtual void drawAt(const Transform2& position) const override {}
    virtual void setBox(const BoundingBox& allowedBox) { m_box = allowedBox; }
    virtual BoundingBox box() const { return m_box; } 
    virtual void registerObject(PropertiesRegisterBuilder* builder) override {}

private:
    BoundingBox m_box;
};

class TopViewController : public ViewController {
public:
    TopViewController() : ViewController(TOP_VIEW_CONTROLLER_ID)
    {
        m_zIndex = std::numeric_limits<unsigned int>::max();
    }

    virtual void load() override
    {
        m_view = std::make_shared<Panel>(
            std::make_shared<FixedOffset>(),
            std::make_shared<TopViewPanelSkin>());

        canvas = std::make_shared<CanvasLayout>(std::make_shared<OffsettedBox>());
        m_view->addObject(canvas);
    }

    std::shared_ptr<CanvasLayout> canvas;
};

void waitAnyKey()
{
    std::cout << "Press any key..." << std::endl;
    char k;
    std::cin >> k;
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

bool Application::init(int* argc, char** argv)
{
    if (m_inited) {
        std::cerr << "Warning: can't init application, already inited" << std::endl;
        return false;
    }

    try {
        configurateFromFile(m_configName.empty() ? DEFAULT_CONFIG_NAME : m_configName);
        const auto& conf = config();
        m_mode = conf.mode;
        if (m_name.empty())
            m_name = conf.windowName;
        if (conf.mode == GraphicsMode::Window)
            initWindowModeInternal(argc, argv, conf.width, conf.height, m_name, 0, 0);
        else
            initGameModeInternal(argc, argv, conf.width, conf.height);
    } catch (std::exception& ex) {
        std::cerr << "Error while initing OpenGL and library core. Reason: " << ex.what() << std::endl;
        return false;
    }

    return initApplication();
}

bool Application::init(int* argc, char** argv, GraphicsMode::Enum mode, int width, int height)
{
    if (m_inited) {
        std::cerr << "Warning: can't init application, already inited" << std::endl;
        return false;
    }

    try {
        configurateFromFile(m_configName.empty() ? DEFAULT_CONFIG_NAME : m_configName);
        m_mode = mode;
        if (mode == GraphicsMode::Window)
            initWindowModeInternal(argc, argv, width, height, m_name, 0, 0);
        else
            initGameModeInternal(argc, argv, width, height);
    } catch (std::exception& ex) {
        std::cerr << "Error while initing OpenGL and library core. Reason: " << ex.what() << std::endl;
        waitAnyKey();
        return false;
    }

    return initApplication();
}

bool Application::initApplication()
{
    try {
        if (m_name.empty())
            m_name = "Gamebase Application";
        m_fpsCounter.reset(new Counter("Frames per 10 seconds", 10.0));

        m_focusedController = nullptr;

        app = this;

        auto topViewController = std::make_shared<TopViewController>();
        registerController(topViewController);
        
        std::cout << "Initing time..." << std::endl;
        TimeState::realTime_.value = currentTime();
        TimeState::realTime_.delta = 0;
        TimeState::gameTime_.value = 0;
        TimeState::gameTime_.delta = 0;

        // calls Application::load(), that creates all ViewControllers
        std::cout << "Initing main view..." << std::endl;
        initView();
        
        std::cout << "Initing views..." << std::endl;
        for (auto it = m_controllers.begin(); it != m_controllers.end(); ++it)
            it->second->initView();

        std::cout << "Building register of objects..." << std::endl;
        m_registerRoot.reset(new RegisterRoot(m_view, m_controllers));
        g_registryBuilder.registerObject(m_registerRoot.get());
        
        std::cout << "Loading resources..." << std::endl;
        loadViewResources();
        for (auto it = m_controllers.begin(); it != m_controllers.end(); ++it)
            it->second->loadViewResources();

        activateControllerByName(TOP_VIEW_CONTROLLER_ID);
        m_topViewLayout = topViewController->canvas.get();

        std::cout << "Registering callbacks..." << std::endl;
        glutDisplayFunc(&gamebase::displayFunc);
        glutKeyboardFunc(&gamebase::keyboardFunc);
        glutKeyboardUpFunc(&gamebase::keyboardUpFunc);
        glutSpecialFunc(&gamebase::specialFunc);
        glutSpecialUpFunc(&gamebase::specialUpFunc);
        glutMotionFunc(&gamebase::motionFunc);
        glutPassiveMotionFunc(&gamebase::motionFunc);
        glutMouseFunc(&gamebase::mouseFunc);
        glutReshapeFunc(&gamebase::resize);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_inited = true;
        std::cout << "Done initing application" << std::endl;
    } catch (std::exception& ex) {
        std::cerr << "Error while initing application. Reason: " << ex.what() << std::endl;
        waitAnyKey();
        return false;
    }
    return true;
}

void Application::setView(const std::shared_ptr<IObject>& obj)
{
    m_view->addObject(obj);
}

void Application::setMode(GraphicsMode::Enum mode)
{
    // ToDo
}

void Application::setScreenSize(int width, int height)
{
    // ToDo
}
    
Size Application::screenSize() const
{
    return Size(
        static_cast<unsigned int>(state().width),
        static_cast<unsigned int>(state().height));
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

    auto newTime = currentTime();
    TimeState::realTime_.delta = newTime - TimeState::realTime_.value;
    TimeState::realTime_.value = newTime;
    TimeState::gameTime_.value++;
    TimeState::gameTime_.delta = 1;

    try {
        processMouseActions();
    
        if (auto keyProcessor = dynamic_cast<IInputProcessor*>(m_selectedObject.lock().get()))
            keyProcessor->processInput(m_inputRegister);
        for (auto it = m_activeControllers.begin(); it != m_activeControllers.end(); ++it)
            (*it)->processInput(m_inputRegister);
    } catch (std::exception& ex)
    {
        std::cerr << "Error while processing input. Reason: " << ex.what() << std::endl;
    }

    try {
        for (auto it = m_activeControllers.begin(); it != m_activeControllers.end(); ++it)
            (*it)->moveView();
    } catch (std::exception& ex)
    {
        std::cerr << "Error while moving. Reason: " << ex.what() << std::endl;
    }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    resetClipper();

    try {
        for (auto it = m_activeControllers.begin(); it != m_activeControllers.end(); ++it)
            (*it)->renderView();
        render();
    } catch (std::exception& ex)
    {
        std::cerr << "Error while rendering. Reason: " << ex.what() << std::endl;
    }

    m_inputRegister.step();

    static std::vector<const AnimationManager*> currentAnimations;
    currentAnimations.clear();
    currentAnimations.insert(currentAnimations.end(),
        g_temp.currentAnimations.begin(), g_temp.currentAnimations.end());
    for (auto it = currentAnimations.begin(); it != currentAnimations.end(); ++it) {
        try {
            (*it)->step();
        } catch (std::exception& ex)
        {
            std::cerr << "Error while running animation. Reason: " << ex.what() << std::endl;
        }
    }

    for (auto it = g_temp.delayedTasks.begin(); it != g_temp.delayedTasks.end(); ++it) {
        try {
            auto& task = *it;
            task();
        } catch (std::exception& ex)
        {
            std::cerr << "Error while executing delayed task. Reason: " << ex.what() << std::endl;
        }
    }
    g_temp.delayedTasks.clear();

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

void Application::restoreSize()
{
    if (m_mode == GraphicsMode::Window) {
        auto& s = state();
        glutReshapeWindow(s.width, s.height);
    }
}

void Application::deactivateAllControllers()
{
    for (auto it = m_activeControllers.begin(); it != m_activeControllers.end(); ++it) {
        if ((*it)->id() == TOP_VIEW_CONTROLLER_ID)
            continue;
        (*it)->deactivate();
    }
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
std::weak_ptr<IObject> filterDisabled(const std::weak_ptr<IObject>& obj)
{
    if (auto selectable = dynamic_cast<ISelectable*>(obj.lock().get())) {
        if (selectable->selectionState() == SelectionState::Disabled)
           return std::weak_ptr<IObject>();
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
        const auto& view = viewController->view();
        auto box = view->box().transformed(view->fullTransform());
        if (box.contains(mousePos)) {
            auto curObject = view->findChildByPoint(mousePos);
            if (!curObject && view->isSelectableByPoint(mousePos))
                curObject = view;
            if (curObject) {
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

namespace {
struct LockedWeakPtr {
    LockedWeakPtr(std::weak_ptr<IObject>& weakPtr)
        : weakPtr(weakPtr)
        , lockedPtr(weakPtr.lock())
        , rawPtr(lockedPtr.get())
    {}

    LockedWeakPtr& operator=(const LockedWeakPtr& other)
    {
        weakPtr = other.weakPtr;
        lockedPtr = other.lockedPtr;
        rawPtr = other.rawPtr;
        return *this;
    }

    ISelectable* selectable() { return dynamic_cast<ISelectable*>(rawPtr); }
    bool isEmpty() const { return rawPtr == nullptr; }
    void reset()
    {
        weakPtr.reset();
        lockedPtr.reset();
        rawPtr = nullptr;
    }
    
    bool operator==(const IObject* obj) const { return rawPtr == obj; }
    bool operator!=(const IObject* obj) const { return !operator==(obj); }
    bool operator==(const LockedWeakPtr& other) const { return operator==(other.rawPtr); }
    bool operator!=(const LockedWeakPtr& other) const { return !operator==(other); }
    bool operator==(const std::weak_ptr<IObject>& obj) const { return operator==(obj.lock().get()); }
    bool operator!=(const std::weak_ptr<IObject>& obj) const { return !operator==(obj); }
    operator bool() { return !isEmpty(); }

    std::weak_ptr<IObject>& weakPtr;
    std::shared_ptr<IObject> lockedPtr;
    IObject* rawPtr;
};
}

void Application::processMouseActions(const std::shared_ptr<IObject>& curObjectSPtr)
{
    std::weak_ptr<IObject> curObjectWPtr(curObjectSPtr);
    curObjectWPtr = filterDisabled(curObjectWPtr);
    LockedWeakPtr curObject(curObjectWPtr);
    LockedWeakPtr mouseOnObject(m_mouseOnObject);
    LockedWeakPtr selectedObject(m_selectedObject);
    LockedWeakPtr associatedSelectable(m_associatedSelectable);

    if (m_inputRegister.mouseButtons.isPressed(MouseButton::Left)) {
        if (m_inputRegister.mouseButtons.isJustPressed(MouseButton::Left)) {
            if (associatedSelectable) {
                bool needReset = false;
                if (auto selectable = curObject.selectable()) {
                    if (associatedSelectable != selectable->associatedSelectable())
                        needReset = true;
                } else {
                    needReset = true;
                }

                if (needReset) {
                    if (curObject != associatedSelectable)
                        associatedSelectable.selectable()->setSelectionState(SelectionState::None);
                    associatedSelectable.reset();
                }
            }

            if (selectedObject && selectedObject != curObject) {
                if (auto selectable = curObject.selectable()) {
                    if (selectedObject == selectable->associatedSelectable()) {
                        associatedSelectable = selectedObject;
                        selectedObject.reset();
                    }
                }
                if (auto selectable = selectedObject.selectable()) {
                    selectable->setSelectionState(SelectionState::None);
                    selectedObject.reset();
                }
            }
            if (mouseOnObject != curObject) {
                changeSelectionState(SelectionState::None);
                mouseOnObject = curObject;
            }
            if (auto selectable = selectedObject.selectable()) {
                if (selectable->selectionState() != SelectionState::Selected
                    && selectable->selectionState() != SelectionState::Pressed) {
                    selectedObject.reset();
                }
            }
            if (auto selectable = mouseOnObject.selectable()) {
                selectable->setSelectionState(SelectionState::Pressed);
                if (selectable->selectionState() == SelectionState::Selected
                    || selectable->selectionState() == SelectionState::Pressed) {
                    selectedObject = mouseOnObject;
                }
            }
        }
    } else {
        if (mouseOnObject != curObject) {
            changeSelectionState(SelectionState::None);
            mouseOnObject = curObject;
            if (auto selectable = mouseOnObject.selectable())
                selectable->setSelectionState(SelectionState::MouseOn);
        }
        if (m_inputRegister.mouseButtons.isJustOutpressed(MouseButton::Left)) {
            bool unselectIfPressed = true;
            if (auto selectable = mouseOnObject.selectable()) {
                if (selectable->selectionState() == SelectionState::Pressed) {
                    selectable->setSelectionState(SelectionState::Selected);
                    if (selectable->selectionState() == SelectionState::Selected
                        || selectable->selectionState() == SelectionState::Pressed) {
                        if (selectedObject && selectedObject != curObject)
                            selectedObject.selectable()->setSelectionState(
                                SelectionState::None);
                        selectedObject = curObject;
                        unselectIfPressed = false;
                    } else {
                        selectedObject.reset();
                    }
                }
            }
            if (auto selectable = selectedObject.selectable()) {
                if (unselectIfPressed && selectable->selectionState() == SelectionState::Pressed) {
                    selectable->setSelectionState(SelectionState::None);
                    selectedObject.reset();
                }
            }
        }
    }
}

void Application::changeSelectionState(SelectionState::Enum state)
{
    LockedWeakPtr mouseOnObject(m_mouseOnObject);
    if (mouseOnObject == m_selectedObject
        || mouseOnObject == m_associatedSelectable)
        return;
    if (auto selectable = mouseOnObject.selectable())
        selectable->setSelectionState(state);
}
}
