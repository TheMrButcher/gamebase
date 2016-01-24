#pragma once

#include <gamebase/engine/ViewController.h>
#include <gamebase/engine/InputRegister.h>
#include <gamebase/graphics/GraphicsMode.h>
#include <gamebase/utils/Counter.h>
#include <map>

namespace gamebase {

class CanvasLayout;

class GAMEBASE_API Application : public ViewController {
public:
    Application();

    void setWindowName(const std::string& name);
    const std::string& configName() const { return m_configName; }
    void setConfigName(const std::string& name) { m_configName = name; }
    bool init(int* argc, char** argv);
    bool init(int* argc, char** argv, GraphicsMode::Enum mode, int width, int height);
    void setMode(GraphicsMode::Enum mode);
    GraphicsMode::Enum mode() const { return m_mode; }
    void setScreenSize(int width, int height);
    Size screenSize() const;

    void run();
    void stop();

    void displayFunc();
    void keyboardFunc(unsigned char key, int, int);
    void keyboardUpFunc(unsigned char key, int, int);
    void specialFunc(int key, int, int);
    void specialUpFunc(int key, int, int);
    void motionFunc(int x, int y);
    void mouseFunc(int button, int state, int x, int y);
    void restoreSize();

    virtual void render() {}

    void deactivateAllControllers();
    void deactivateControllerByName(const std::string& controllerName);
    void deactivateController(ViewController* controller);
    void activateControllerByName(const std::string& controllerName);
    void activateController(ViewController* controller);

    virtual void processKeyDown(unsigned char key) {}
    virtual void processKeyUp(unsigned char key) {}
    virtual void processSpecialKeyDown(int key) {}
    virtual void processSpecialKeyUp(int key) {}
    virtual void processMouseMotion(const Vec2& pos) {}
    virtual void processMouseButtonDown(MouseButton::Enum button) {}
    virtual void processMouseButtonUp(MouseButton::Enum button) {}

    const InputRegister& inputRegister() { return m_inputRegister; }
    CanvasLayout* topViewLayout() { return m_topViewLayout; }

protected:
    bool initApplication();
    void registerController(const std::shared_ptr<ViewController>& controller);
    void sortControllers();
    ViewController* currentController();
    void setFocus(ViewController* controller);
    void filterControllers();

    void processMouseActions();
    void processMouseActions(const std::shared_ptr<IObject>& curObject);
    void changeSelectionState(SelectionState::Enum state);

    bool m_inited;
    std::string m_configName;
    std::string m_name;
    GraphicsMode::Enum m_mode;
    std::unique_ptr<Counter> m_fpsCounter;
    InputRegister m_inputRegister;

    std::weak_ptr<IObject> m_mouseOnObject;
    std::weak_ptr<IObject> m_selectedObject;
    std::weak_ptr<IObject> m_associatedSelectable;

    std::map<std::string, std::shared_ptr<ViewController>> m_controllers;
    std::vector<ViewController*> m_activeControllers;
    ViewController* m_focusedController;
    CanvasLayout* m_topViewLayout;

    std::unique_ptr<IRegistrable> m_registerRoot;
};

GAMEBASE_API extern Application* app;

}
