#pragma once

#include <gamebase/impl/ui/Button.h>

namespace gamebase { namespace impl {

class GAMEBASE_API ToggleButton : public Button {
public:
    ToggleButton(
        const std::shared_ptr<ButtonSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr)
        : Button(skin, position)
        , m_pressed(false)
        , m_unpressOnFocusLost(true)
    {}

    void setUnpressOnFocusLost(bool value) { m_unpressOnFocusLost = value; }

    void setPressed(bool value);
    bool isPressed() const { return m_pressed; }

    virtual void setSelectionState(SelectionState::Enum state) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    bool m_pressed;
    bool m_unpressOnFocusLost;
};

} }
