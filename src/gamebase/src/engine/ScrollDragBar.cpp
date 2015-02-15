#include <stdafx.h>
#include <gamebase/engine/ScrollDragBar.h>

namespace gamebase {

ScrollDragBar::ScrollDragBar(
    const std::shared_ptr<FixedOffset>& position,
    const std::shared_ptr<ScrollDragBarSkin>& skin,
    const std::shared_ptr<FloatValue>& controlledValue)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
    , m_skin(skin)
    , m_controlledValue(controlledValue)
{}

void ScrollDragBar::setSelectionState(SelectionState::Enum state)
{
    if (state == SelectionState::Selected)
        state = SelectionState::MouseOn;
    if (state != SelectionState::Pressed)
        m_mousePos.reset();
    m_selectionState = state;
    m_skin->setSelectionState(state);
}

void ScrollDragBar::processInput(const InputRegister& input)
{
    if (m_selectionState == SelectionState::Pressed) {
        float coord = m_skin->direction() == Direction::Horizontal
            ? input.mousePosition().x : input.mousePosition().y;
        if (m_mousePos) {
            m_controlledValue->set(coord - *m_mousePos);
        } else {
            m_controlledValue->set(0.0f);
            m_mousePos.reset(coord);
        }
    }
}

}
