#include <stdafx.h>
#include <gamebase/engine/ScrollDragBar.h>

namespace gamebase {

ScrollDragBar::ScrollDragBar(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<ScrollDragBarSkin>& skin)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
    , m_skin(skin)
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
        Vec2 pos = input.mousePosition();
        Vec2 delta;
        if (m_mousePos) {
            delta = pos - *m_mousePos;
        } else {
            m_mousePos.reset(pos);
        }
        if (m_controlledHorizontal)
            m_controlledHorizontal->set(delta.x);
        if (m_controlledVertical)
            m_controlledVertical->set(delta.y);
    }
}

void ScrollDragBar::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject("skin", m_skin.get());
}
}
