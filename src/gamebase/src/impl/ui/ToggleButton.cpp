/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/ui/ToggleButton.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

ToggleButton::~ToggleButton()
{
    m_callbackHandle.cancel();
}

void ToggleButton::setPressed(bool value)
{
    if (m_unpressOnFocusLost || m_pressed == value)
        return;
    m_pressed = value;
    if (m_callback)
        m_callback();
}

void ToggleButton::setSelectionState(SelectionState::Enum state)
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
        if (!m_pressed && m_callback)
            m_callbackHandle = Handle(m_callback);
    } else {
        m_selectionState = state;
        m_skin->setSelectionState(state);
        if (state == SelectionState::Pressed) {
            m_pressed = true;
            if (m_callback)
                m_callbackHandle = Handle(m_callback);
        }
    }
}

void ToggleButton::serialize(Serializer& s) const
{
    s << "unpressOnFocusLost" << m_unpressOnFocusLost << "pressed" << m_pressed;
    Button::serialize(s);
}

std::unique_ptr<IObject> deserializeToggleButton(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, skin);
    DESERIALIZE(bool, unpressOnFocusLost);
    std::unique_ptr<ToggleButton> result(new ToggleButton(skin, position));
    result->setUnpressOnFocusLost(unpressOnFocusLost);
    if (!unpressOnFocusLost) {
        DESERIALIZE(bool, pressed);
        result->setPressed(pressed);
    }
    return std::move(result);
}

REGISTER_CLASS(ToggleButton);

} }
