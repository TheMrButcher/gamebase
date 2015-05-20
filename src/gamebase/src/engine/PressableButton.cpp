#include <stdafx.h>
#include <gamebase/engine/PressableButton.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

void PressableButton::setSelectionState(SelectionState::Enum state)
{
    auto curState = m_selectionState;
    if (curState == SelectionState::Selected) {
        if (state == SelectionState::MouseOn)
            return;
        if (state != SelectionState::None)
            state = SelectionState::MouseOn;
        Button::setSelectionState(state);
        if (m_unpressCallback)
            m_unpressCallback();
    } else {
        if (state == SelectionState::Pressed)
            state = SelectionState::Selected;
        Button::setSelectionState(state);
        if (state == SelectionState::Selected) {
            Button::setSelectionState(SelectionState::Pressed);
            m_selectionState = state;
        }
    }
}

IObject* deserializePressableButton(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, skin);
    return new PressableButton(position, skin);
}

REGISTER_CLASS(PressableButton);

}
