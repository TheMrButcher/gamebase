#include <stdafx.h>
#include <gamebase/engine/Button.h>

namespace gamebase {

Button::Button(
    const Vec2& position,
    const std::shared_ptr<IGeometry>& geom,
    const std::shared_ptr<Skin>& skin,
    const std::function<void()>& callback)
    : OffsettedPosition(position)
    , FindableGeometry(this, geom)
    , m_skin(skin)
    , m_callback(callback)
{}

void Button::setSelectionState(SelectionState::Enum state)
{
    if (state == SelectionState::Selected) {
        state = SelectionState::MouseOn;
        if (m_callback)
            m_callback();
    }

    m_selectionState = state;
    m_skin->setSelectionState(state);
}

}
