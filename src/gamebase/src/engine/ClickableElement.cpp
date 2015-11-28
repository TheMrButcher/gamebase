#include <stdafx.h>
#include <gamebase/engine/ClickableElement.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

void ClickableElement::setSelectionState(SelectionState::Enum state)
{
    bool clicked = false;
    if (state == SelectionState::Selected) {
        state = SelectionState::MouseOn;
        clicked = true;
    }
    m_selectionState = state;
    if (m_transitionCallback)
        m_transitionCallback(state);
    if (clicked && m_callback)
        m_callback();
}

std::unique_ptr<IObject> deserializeClickableElement(Deserializer&)
{
    return std::unique_ptr<ClickableElement>(new ClickableElement());
}

REGISTER_CLASS(ClickableElement);

}
