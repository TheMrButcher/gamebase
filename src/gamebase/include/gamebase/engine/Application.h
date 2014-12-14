#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/InputRegister.h>
#include <gamebase/engine/IObject.h>
#include <gamebase/engine/ISelectable.h>
#include <gamebase/engine/ObjectsCollection.h>
#include <gamebase/utils/Counter.h>

namespace gamebase {

class GAMEBASE_API Application {
public:
    Application();

    void setWindowName(const std::string& name);

    enum Mode {
        Window,
        Game
    };
    bool init(int* argc, char** argv, Mode mode, int width, int height);
    void setMode(Mode mode);
    void setScreenSize(int width, int height);
    void setMoveTime(float time);

    void run();
    void stop();

    void displayFunc();
    void keyboardFunc(unsigned char key, int, int);
    void keyboardUpFunc(unsigned char key, int, int);
    void specialFunc(int key, int, int);
    void specialUpFunc(int key, int, int);
    void motionFunc(int x, int y);
    void mouseFunc(int button, int state, int x, int y);

    virtual void load() {}
    virtual void processInput() {}
    virtual void render() {}
    virtual void move() {}
    virtual void processKeyDown(unsigned char key) {}
    virtual void processKeyUp(unsigned char key) {}
    virtual void processSpecialKeyDown(int key) {}
    virtual void processSpecialKeyUp(int key) {}
    virtual void processMouseMotion(const Vec2& pos) {}
    virtual void processMouseButtonDown(MouseButton::Enum button) {}
    virtual void processMouseButtonUp(MouseButton::Enum button) {}

protected:
    void processMouseActions();
    void changeSelectionState(SelectionState::Enum state);

    bool m_inited;
    std::string m_name;
    Mode m_mode;
    std::unique_ptr<Counter> m_fpsCounter;
    InputRegister m_inputRegister;
    ObjectsCollection m_rootObject;
    float m_moveTime;

    IObject* m_mouseOnObject;
    IObject* m_selectedObject;
};

}
