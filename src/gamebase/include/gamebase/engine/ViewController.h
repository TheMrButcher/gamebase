#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/Panel.h>

namespace gamebase {

class GAMEBASE_API ViewController {
public:
    enum State {
        Invisible,
        Inactive,
        Visible,
        Focused
    };

    ViewController(const std::string& id);
    virtual ~ViewController();
    
    const std::string& id() const { return m_id; }
    const std::shared_ptr<Panel>& view() { return m_view; }
    State viewState() const { return m_viewState; }
    unsigned int zIndex() const { return m_zIndex; }

    virtual void initView();
    void loadViewResources();
    void renderView();
    void moveView();
    
    virtual void activate()   { if (m_viewState == Invisible) m_viewState = Visible; }
    virtual void deactivate() {                               m_viewState = Invisible; }
    virtual void focused()    { if (m_viewState == Visible)   m_viewState = Focused; }
    virtual void lostFocus()  { if (m_viewState == Focused)   m_viewState = Visible; }

    virtual void load() {}
    virtual void postload() {}
    virtual void move() {}
    virtual void processInput(const InputRegister& inputRegister) {}

protected:
    std::string m_id;
    State m_viewState;
    unsigned int m_zIndex;
    std::shared_ptr<Panel> m_view;
};

}
