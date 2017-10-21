/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/ui/CheckBox.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

CheckBox::CheckBox(
    const std::shared_ptr<CheckBoxSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
    , m_skin(skin)
    , m_checked(false)
{
    m_skin->setChecked(false);
}

void CheckBox::setCallback(const std::function<void()>& callback)
{
	setCallback([callback](bool) { callback(); });
}

void CheckBox::setChecked(bool status)
{
    if (m_checked != status) {
        m_checked = status;
        m_skin->setChecked(status);
        if (m_callback)
            m_callbackHandle = Handle([this, status]() { m_callback(status); });
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
    builder->registerObject(m_skin.get());
    builder->registerProperty("check", &m_checked,
		[this](bool value) { setChecked(value); });
}

void CheckBox::serialize(Serializer& s) const
{
    s << "checked" << m_checked << "position" << m_offset << "skin" << m_skin;
}

std::unique_ptr<IObject> deserializeCheckBox(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<CheckBoxSkin>, skin);
    DESERIALIZE(bool, checked);
    std::unique_ptr<CheckBox> result(new CheckBox(skin, position));
    result->setChecked(checked);
    return std::move(result);
}

REGISTER_CLASS(CheckBox);

} }
