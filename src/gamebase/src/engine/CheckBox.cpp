#include <stdafx.h>
#include <gamebase/engine/CheckBox.h>

namespace gamebase {

CheckBox::CheckBox(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<CheckBoxSkin>& skin)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
    , m_skin(skin)
    , m_checked(false)
{
    m_skin->setChecked(false);
}

void CheckBox::setChecked(bool status)
{
    if (m_checked != status) {
        m_checked = status;
        m_skin->setChecked(status);
        m_callback(status);
    }
}

void CheckBox::setSelectionState(SelectionState::Enum state)
{
    if (state == SelectionState::Selected) {
        state = SelectionState::MouseOn;
        setChecked(!m_checked);
    }

    m_selectionState = state;
    m_skin->setSelectionState(state);
}

}
