/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/app/Application.h>
#include "KeyConverter.h"
#include "src/impl/global/Config.h"
#include "src/impl/global/GlobalTemporary.h"
#include "src/impl/global/GlobalResources.h"
#include "src/impl/global/GlobalCache.h"
#include "src/impl/graphics/State.h"
#include "src/impl/graphics/InitInternal.h"
#include <gamebase/impl/app/TimeState.h>
#include <gamebase/impl/app/Config.h>
#include <gamebase/impl/engine/IDrawable.h>
#include <gamebase/impl/engine/IMovable.h>
#include <gamebase/impl/engine/IInputProcessor.h>
#include <gamebase/impl/ui/CanvasLayout.h>
#include <gamebase/impl/relbox/OffsettedBox.h>
#include <gamebase/impl/graphics/Clipping.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

namespace gamebase { namespace impl {
Application* app;
TimeState TimeState::realTime_;
TimeState TimeState::gameTime_;

const std::string TOP_VIEW_CONTROLLER_ID = "app_top";

namespace {
enum MouseState {
    MouseButtonDown,
    MouseButtonUp
};

class RegisterRoot : public Registrable {
public:
    RegisterRoot(
        std::shared_ptr<Panel>& appView,
        std::map<std::string, std::shared_ptr<ViewController>>& views)
        : m_appView(appView)
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
    , m_isRunning(false)
    , m_frameNum(0)
    , m_loadTime(0)
{
    std::cout << "Initing time..." << std::endl;
    TimeState::realTime_.value = currentTime();
    TimeState::realTime_.delta = 0;
    TimeState::gameTime_.value = 0;
    TimeState::gameTime_.delta = 0;
}

Application::~Application()
{
    g_temp.audioManager.reset();
    g_temp.delayedTasks.clear();
    g_temp.callOnceTimers.clear();
    g_temp.timers.clear();
    globalResources().fontStorage.clear();
    globalResources().soundLibrary.clear();
    g_cache.designCache.clear();
    g_cache.textureCache.clear();
}

void Application::setWindowTitle(const std::string& title)
{
    m_window.setTitle(title);
}

bool Application::init(int* argc, char** argv)
{
    if (m_window.isInited()) {
        std::cerr << "Warning: can't init application, already inited" << std::endl;
        return false;
    }

    try {
        configurateFromFile(m_configName.empty() ? DEFAULT_CONFIG_NAME : m_configName);
        const auto& conf = config();
        if (m_window.title().empty())
            m_window.setTitle(conf.windowTitle);
        setMode(conf.mode);
        m_window.setSize(static_cast<unsigned int>(conf.width), static_cast<unsigned int>(conf.height));
        m_window.init(argc, argv);
    } catch (std::exception& ex) {
        std::cerr << "Error while initing OpenGL and library core. Reason: " << ex.what() << std::endl;
        m_window.destroy();
        waitAnyKey();
        return false;
    }

    return initApplication();
}

bool Application::initOverrideConfig(int* argc, char** argv)
{
    if (m_window.isInited()) {
        std::cerr << "Warning: can't init application, already inited" << std::endl;
        return false;
    }

    try {
        configurateFromFile(m_configName.empty() ? DEFAULT_CONFIG_NAME : m_configName);
        m_window.init(argc, argv);
    } catch (std::exception& ex) {
        std::cerr << "Error while initing OpenGL and library core. Reason: " << ex.what() << std::endl;
        m_window.destroy();
        waitAnyKey();
        return false;
    }

    return initApplication();
}

bool Application::initApplication()
{
    try {
        m_window.getImpl()->setActive(true);
        m_fpsCounter.reset(new Counter("FPS", 5.0));

        m_focusedController = nullptr;

        app = this;

        auto topViewController = std::make_shared<TopViewController>();
        registerController(topViewController);

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

        m_topViewLayout = topViewController->canvas.get();

        std::cout << "Postload..." << std::endl;
        postload();
        for (auto it = m_controllers.begin(); it != m_controllers.end(); ++it)
            it->second->postload();

        activateControllerByName(TOP_VIEW_CONTROLLER_ID);

        std::cout << "Done initing application" << std::endl;
    } catch (std::exception& ex) {
        std::cerr << "Error while initing application. Reason: " << ex.what() << std::endl;
        m_window.destroy();
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
    m_window.setMode(mode);
}

void Application::setWindowSize(unsigned int width, unsigned int height)
{
    m_pendingWindowSize = Size(width, height);
}

Size Application::windowSize() const
{
    if (m_pendingWindowSize)
        return *m_pendingWindowSize;
    return m_window.size();
}

void Application::setMinWindowSize(unsigned int w, unsigned int h)
{
    m_window.setMinSize(w, h);
    m_pendingWindowSize = m_window.size();
}

void Application::setMaxWindowSize(unsigned int w, unsigned int h)
{
    m_window.setMaxSize(w, h);
    m_pendingWindowSize = m_window.size();
}

void Application::run()
{
    m_isRunning = true;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    while (m_isRunning) {
        if (m_pendingWindowSize) {
            resizeFunc(*m_pendingWindowSize);
            m_pendingWindowSize = boost::none;
        }

        sf::Event e;
        while (m_window.getImpl()->pollEvent(e)) {
            switch (e.type) {
            case sf::Event::Closed:
                if (onClose())
                    close();
                continue;

            case sf::Event::Resized:
                resizeFunc(Size(e.size.width, e.size.height));
                continue;

            case sf::Event::TextEntered:
                textFunc(e.text.unicode);
                continue;

            case sf::Event::KeyPressed:
                keyboardFunc(e.key.code);
                continue;

            case sf::Event::KeyReleased:
                keyboardUpFunc(e.key.code);
                continue;

            case sf::Event::MouseWheelScrolled:
                wheelFunc(
                    e.mouseWheelScroll.wheel, e.mouseWheelScroll.delta,
                    e.mouseWheelScroll.x, e.mouseWheelScroll.y);
                continue;

            case sf::Event::MouseButtonPressed:
                mouseFunc(
                    e.mouseButton.button, MouseButtonDown,
                    e.mouseButton.x, e.mouseButton.y);
                continue;

            case sf::Event::MouseButtonReleased:
                mouseFunc(
                    e.mouseButton.button, MouseButtonUp,
                    e.mouseButton.x, e.mouseButton.y);
                continue;

            case sf::Event::MouseMoved:
                motionFunc(e.mouseMove.x, e.mouseMove.y);
                continue;
            }
        }
        displayFunc();
    }

    // ToDo: add onTerminate
    m_window.getImpl()->close();
}

void Application::close()
{
    m_isRunning = false;
}

void Application::displayFunc()
{
    filterControllers();

    if (m_fpsCounter)
        m_fpsCounter->touch();

    if (m_frameNum > 0) {
        auto newTime = currentTime() - m_loadTime;
        TimeState::realTime_.delta = newTime - TimeState::realTime_.value;
        TimeState::realTime_.value = newTime;
    } else {
        m_loadTime = currentTime() - TimeState::realTime_.value;
    }
    TimeState::gameTime_.value++;
    TimeState::gameTime_.delta = 1;

    ++m_frameNum;

    try {
        processMouseActions();

        auto mouseOnObject = m_mouseOnObject.lock();
        auto selectedObject = m_selectedObject.lock();
        if (auto keyProcessor = dynamic_cast<IInputProcessor*>(mouseOnObject.get()))
            keyProcessor->processInput(m_inputRegister);
        if (mouseOnObject != selectedObject) {
            if (auto keyProcessor = dynamic_cast<IInputProcessor*>(selectedObject.get()))
                keyProcessor->processInput(m_inputRegister);
        }
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

    static std::vector<const AnimationManager*> currentAnimations;
    currentAnimations.clear();
    currentAnimations.assign(g_temp.currentAnimations.begin(), g_temp.currentAnimations.end());
    for (auto it = currentAnimations.begin(); it != currentAnimations.end(); ++it) {
        try {
            (*it)->step();
        } catch (std::exception& ex)
        {
            std::cerr << "Error while running animation. Reason: " << ex.what() << std::endl;
        }
    }

    for (size_t i = 0; i < g_temp.delayedTasks.size(); ++i) {
        try {
            auto task = g_temp.delayedTasks[i];
            if (task)
                task();
        } catch (std::exception& ex)
        {
            std::cerr << "Error while executing delayed task. Reason: " << ex.what() << std::endl;
        }
    }
    g_temp.delayedTasks.clear();

    for (size_t i = 0; i < g_temp.timers.size();) {
        if (g_temp.timers[i].expired()) {
            std::swap(g_temp.timers[i], g_temp.timers.back());
            g_temp.timers.pop_back();
            continue;
        }

        auto timer = g_temp.timers[i].lock();
        if (!timer->isPeriodical()) {
            timer->setInQueue(false);
            std::swap(g_temp.timers[i], g_temp.timers.back());
            g_temp.timers.pop_back();
            continue;
        }

        try {
            while (timer->shiftPeriodInQueue());
        }
        catch (std::exception& ex)
        {
            std::cerr << "Error while executing timer callback. Reason: " << ex.what() << std::endl;
        }
        ++i;
    }

    try {
        g_temp.activeAudio.step();
        g_temp.audioManager.step();
    } catch (std::exception& ex)
    {
        std::cerr << "Error while processing sounds. Reason: " << ex.what() << std::endl;
    }

    m_inputRegister.step();

    m_window.getImpl()->display();
}

void Application::resizeFunc(Size size)
{
    auto oldSize = m_window.size();
    m_window.setSize(size.width, size.height);
    size = m_window.size();
    if (size == oldSize)
        return;
    glViewport(0, 0, size.width, size.height);
    initState(static_cast<int>(size.width), static_cast<int>(size.height));
    std::cout << "Loading resources..." << std::endl;
    loadViewResources();
    for (auto it = m_controllers.begin(); it != m_controllers.end(); ++it)
        it->second->loadViewResources();
    onResize(size);
}

void Application::keyboardFunc(int sfKey)
{
    auto key = convertCode(sfKey);
    m_inputRegister.keys.setDown(key);
    processKeyDown(key);
}

void Application::keyboardUpFunc(int sfKey)
{
    auto key = convertCode(sfKey);
    m_inputRegister.keys.setUp(key);
    processKeyUp(key);
}

void Application::textFunc(uint32_t unicodeKey)
{
    m_inputRegister.chars.push_back(unicodeKey);
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
    InputKey::Enum button;
    switch (buttonCode) {
    case sf::Mouse::Left:   button = InputKey::MouseLeft; break;
    case sf::Mouse::Middle: button = InputKey::MouseMiddle; break;
    case sf::Mouse::Right:  button = InputKey::MouseRight; break;
    default: return;
    }
    if (state == MouseButtonDown) {
        m_inputRegister.keys.setDown(button);
        processMouseButtonDown(button);
    } else {
        m_inputRegister.keys.setUp(button);
        processMouseButtonUp(button);
    }
}

void Application::wheelFunc(int, float offset, int, int)
{
    m_inputRegister.wheel += offset;
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
        auto box = view->box();
        box.transform(view->fullTransform());
        if (box.contains(mousePos)) {
            auto curObject = view->findChildByPoint(mousePos);
            if (!curObject && view->isSelectableByPoint(mousePos))
                curObject = view;
            if (curObject) {
                if (viewController->viewState() != ViewController::Inactive) {
                    if (m_inputRegister.keys.isJustPressed(InputKey::MouseLeft))
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

    if (m_inputRegister.keys.isPressed(InputKey::MouseLeft)) {
        if (m_inputRegister.keys.isJustPressed(InputKey::MouseLeft)) {
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
        if (m_inputRegister.keys.isJustOutpressed(InputKey::MouseLeft)) {
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
} }
