/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/ui/RadioButton.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

RadioButton::RadioButton(
    const std::shared_ptr<CheckBoxSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
    , m_skin(skin)
    , m_checked(false)
    , m_index(-1)
{
    m_skin->setChecked(false);
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

void RadioButton::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject("skin", m_skin.get());
    builder->registerPropertyWithSetter("checked", &m_checked, [this](bool status)
    {
        if (!status)
            THROW_EX() << "Can't uncheck RadioButton. Check another RadioButton to uncheck this";
        setChecked();
    });
}

void RadioButton::serialize(Serializer& s) const
{
    s << "index" << m_index << "position" << m_offset << "skin" << m_skin;
}

std::unique_ptr<IObject> deserializeRadioButton(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<CheckBoxSkin>, skin);
    DESERIALIZE(int, index);
    std::unique_ptr<RadioButton> result(new RadioButton(skin, position));
    result->setIndexInGroup(index);
    return std::move(result);
}

REGISTER_CLASS(RadioButton);

} }
