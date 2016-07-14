/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/ui/ScrollDragBar.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

ScrollDragBar::ScrollDragBar(
    const std::shared_ptr<ButtonSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
    , m_skin(skin)
{}

void ScrollDragBar::setSelectionState(SelectionState::Enum state)
{
    if (state == SelectionState::Selected)
        state = SelectionState::MouseOn;
    if (state != SelectionState::Pressed)
        m_mousePos.reset();
    m_selectionState = state;
    m_skin->setSelectionState(state);
}

void ScrollDragBar::processInput(const InputRegister& input)
{
    if (m_selectionState == SelectionState::Pressed) {
        Vec2 pos = input.mousePosition();
        Vec2 delta;
        if (m_mousePos) {
            delta = pos - *m_mousePos;
        } else {
            m_mousePos.reset(pos);
        }
        if (m_controlledHorizontal)
            m_controlledHorizontal->set(delta.x);
        if (m_controlledVertical)
            m_controlledVertical->set(delta.y);
    }
}

void ScrollDragBar::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject("skin", m_skin.get());
}

void ScrollDragBar::serialize(Serializer& s) const
{
    s << "position" << m_offset << "skin" << m_skin;
}

std::unique_ptr<IObject> deserializeScrollDragBar(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, skin);
    return std::unique_ptr<IObject>(new ScrollDragBar(skin, position));
}

REGISTER_CLASS(ScrollDragBar);

} }
