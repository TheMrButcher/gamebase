/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/ui/Button.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

Button::Button(
    const std::shared_ptr<ButtonSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
    , m_skin(skin)
{}

void Button::setSelectionState(SelectionState::Enum state)
{
    bool clicked = false;
    if (state == SelectionState::Selected) {
        state = SelectionState::MouseOn;
        clicked = true;
    }
    m_selectionState = state;
    m_skin->setSelectionState(state);
    if (clicked && m_callback)
        m_callbackHandle = Handle(m_callback);
}

void Button::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject(m_skin.get());
}

void Button::serialize(Serializer& s) const
{
    s << "position" << m_offset << "skin" << m_skin;
}

std::unique_ptr<IObject> deserializeButton(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, skin);
    return std::unique_ptr<IObject>(new Button(skin, position));
}

REGISTER_CLASS(Button);

} }
