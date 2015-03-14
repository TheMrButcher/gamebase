#include <stdafx.h>
#include <gamebase/engine/RadioButton.h>

namespace gamebase {

RadioButton::RadioButton(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<CheckBoxSkin>& skin,
    const std::shared_ptr<RadioButtonGroup>& group)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
    , m_skin(skin)
    , m_checked(false)
    , m_index(0)
{
    m_skin->setChecked(false);
    setGroup(group);
}

void RadioButton::setGroup(const std::shared_ptr<RadioButtonGroup>& group)
{
    if (m_group)
        THROW_EX() << "Can't replace RadioButtonGroup: not implemented";
    m_group = group;
    m_index = m_group->add(this);
}

void RadioButton::setChecked()
{
    if (m_checked != true) {
        setCheckedImpl(true);
        if (m_group)
            m_group->setSelected(m_index);
    }
}

void RadioButton::setSelectionState(SelectionState::Enum state)
{
    if (state == SelectionState::Selected) {
        state = SelectionState::MouseOn;
        if (!m_checked)
            setChecked();
    }

    m_selectionState = state;
    m_skin->setSelectionState(state);
}

void RadioButton::setCheckedImpl(bool status)
{
    m_checked = status;
    m_skin->setChecked(status);
}

namespace {
void checkedPropertySetter(RadioButton* button, bool status)
{
    if (!status)
        THROW_EX() << "Can't uncheck RadioButton. Check another RadioButton to uncheck this";
    button->setChecked();
}
}

void RadioButton::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject("skin", m_skin.get());
    builder->registerProperty("checked", &m_checked,
        std::bind(&checkedPropertySetter, this, std::placeholders::_1));
}

}
