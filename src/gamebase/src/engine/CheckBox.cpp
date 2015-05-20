#include <stdafx.h>
#include <gamebase/engine/CheckBox.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

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

void CheckBox::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject("skin", m_skin.get());
    builder->registerProperty("checked", &m_checked,
        std::bind(&CheckBox::setChecked, this, std::placeholders::_1));
}

void CheckBox::serialize(Serializer& s) const
{
    s << "position" << m_offset << "skin" << m_skin << "checked" << m_checked;
}

IObject* deserializeCheckBox(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<CheckBoxSkin>, skin);
    DESERIALIZE(bool, checked);
    auto* result = new CheckBox(position, skin);
    result->setChecked(checked);
    return result;
}

REGISTER_CLASS(CheckBox);

}
