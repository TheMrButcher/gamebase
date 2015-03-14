#include <stdafx.h>
#include <gamebase/engine/Button.h>

namespace gamebase {

Button::Button(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<ButtonSkin>& skin,
    const std::function<void()>& callback)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
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

void Button::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject("skin", m_skin.get());
}

}
