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

void Button::setToolTip(const std::shared_ptr<ToolTip>& toolTip)
{
    m_toolTip = toolTip;
    if (m_toolTip)
        m_toolTip->setParentPosition(this);
}

void Button::setSelectionState(SelectionState::Enum state)
{
    bool clicked = false;
    if (state == SelectionState::Selected) {
        state = SelectionState::MouseOn;
        clicked = true;
    }
    m_selectionState = state;
    m_skin->setSelectionState(state);
    if (m_toolTip) {
        m_toolTip->setVisible(
            m_selectionState != SelectionState::None
            && m_selectionState != SelectionState::Disabled);
    }
    if (clicked && m_callback)
        m_callbackHandle = Handle(m_callback);
}

void Button::loadResources()
{
    m_skin->loadResources();
    if (m_toolTip)
        m_toolTip->loadResources();
}

void Button::drawAt(const Transform2& position) const
{
    m_skin->draw(position);
    if (m_toolTip)
        m_toolTip->draw(position);
}

void Button::setBox(const BoundingBox& allowedBox)
{
    m_skin->setBox(allowedBox);
    if (m_toolTip)
        m_toolTip->setBox(box());
    setPositionBoxes(allowedBox, box());
}

void Button::processInput(const InputRegister&)
{
    if (!m_toolTip)
        return;

    m_toolTip->setSourcePoint(gamebase::impl::mouseCoords(this));
}

void Button::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject(m_skin.get());
    if (m_toolTip)
        builder->registerObject(m_toolTip.get());
}

void Button::serialize(Serializer& s) const
{
    s << "position" << m_offset;
    s << "skin" << m_skin;
    if (s.mode() == impl::SerializationMode::ForcedFull || m_toolTip)
        s << "toolTip" << m_toolTip;
}

std::unique_ptr<IObject> deserializeButton(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, skin);
    auto result = std::make_unique<Button>(skin, position);
    if (deserializer.hasMember("toolTip")) {
        DESERIALIZE(std::shared_ptr<ToolTip>, toolTip);
        result->setToolTip(toolTip);
    }
    return result;
}

REGISTER_CLASS(Button);

} }
