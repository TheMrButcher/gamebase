#pragma once

#include <gamebase/engine/Button.h>

namespace gamebase {

class GAMEBASE_API PressableButton : public Button {
public:
    PressableButton(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<ButtonSkin>& skin,
        const std::function<void()>& callback = nullptr)
        : Button(position, skin, callback)
        , m_pressed(false)
        , m_unpressOnFocusLost(true)
    {}

    void setUnpressOnFocusLost(bool value) { m_unpressOnFocusLost = value; }

    void setPressed(bool value);
    bool isPressed() const { return m_pressed; }

    void setUnpressCallback(const std::function<void()>& callback)
    {
        m_unpressCallback = callback;
    }

    virtual void setSelectionState(SelectionState::Enum state) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    std::function<void()> m_unpressCallback;
    bool m_pressed;
    bool m_unpressOnFocusLost;
};

}
