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
    {}

    void setUnpressCallback(const std::function<void()>& callback)
    {
        m_unpressCallback = callback;
    }

    virtual void setSelectionState(SelectionState::Enum state) override;

private:
    std::function<void()> m_unpressCallback;
};

}
