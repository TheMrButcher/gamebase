#include <stdafx.h>
#include <gamebase/engine/PressableButton.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

void PressableButton::setPressed(bool value)
{
    if (m_unpressOnFocusLost || m_pressed == value)
        return;
    m_pressed = value;
    if (m_pressed) {
        if (m_callback)
            m_callback();
    } else {
        if (m_unpressCallback)
            m_unpressCallback();
    }
}


void PressableButton::setSelectionState(SelectionState::Enum state)
{
    auto curState = m_selectionState;
    if (state == SelectionState::Selected)
        state = SelectionState::MouseOn;
    if (m_pressed) {
        if (m_unpressOnFocusLost) {
            if (state == SelectionState::MouseOn)
                return;
            if (state == SelectionState::Pressed) {
                m_selectionState = SelectionState::Pressed;
                m_skin->setSelectionState(SelectionState::None);
            } else {
                m_selectionState = state;
                m_skin->setSelectionState(state);
            }
            m_pressed = false;
        } else {
            if (state == SelectionState::Pressed) {
                state = SelectionState::Pressed;
                m_skin->setSelectionState(SelectionState::None);
                m_pressed = false;
            }
            m_selectionState = state;
        }
        if (!m_pressed && m_unpressCallback)
            m_unpressCallback();
    } else {
        m_selectionState = state;
        m_skin->setSelectionState(state);
        if (state == SelectionState::Pressed) {
            m_pressed = true;
            if (m_callback)
                m_callback();
        }
    }
}

void PressableButton::serialize(Serializer& s) const
{
    Button::serialize(s);
    s << "unpressOnFocusLost" << m_unpressOnFocusLost;
    if (!m_unpressOnFocusLost)
        s << "pressed" << m_pressed;
}

std::unique_ptr<IObject> deserializePressableButton(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, skin);
    DESERIALIZE(bool, unpressOnFocusLost);
    std::unique_ptr<PressableButton> result(new PressableButton(position, skin));
    result->setUnpressOnFocusLost(unpressOnFocusLost);
    if (!unpressOnFocusLost) {
        DESERIALIZE(bool, pressed);
        result->setPressed(pressed);
    }
    return std::move(result);
}

REGISTER_CLASS(PressableButton);

}
