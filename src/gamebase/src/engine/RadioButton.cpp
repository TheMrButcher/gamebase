#include <stdafx.h>
#include <gamebase/engine/RadioButton.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

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
    , m_index(-1)
{
    m_skin->setChecked(false);
    if (group)
        setGroup(group);
}

RadioButton::~RadioButton()
{
    if (m_group)
        m_group->remove(m_index);
}

void RadioButton::setGroup(const std::shared_ptr<RadioButtonGroup>& group)
{
    if (m_group)
        THROW_EX() << "Can't replace RadioButtonGroup: not implemented";
    m_group = group;
    if (m_index == -1)
        m_index = m_group->add(this);
    else
        m_group->insert(m_index, this);
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

void RadioButton::serialize(Serializer& s) const
{
    s << "position" << m_offset << "skin" << m_skin << "index" << m_index;
}

std::unique_ptr<IObject> deserializeRadioButton(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<CheckBoxSkin>, skin);
    DESERIALIZE(int, index);
    std::unique_ptr<RadioButton> result(new RadioButton(position, skin));
    result->setIndexInGroup(index);
    return std::move(result);
}

REGISTER_CLASS(RadioButton);

}
