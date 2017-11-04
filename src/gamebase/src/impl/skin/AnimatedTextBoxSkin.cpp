/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/AnimatedTextBoxSkin.h>
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

namespace {
const Time DEFAULT_SHIFT_PERIOD = 100;
}

AnimatedTextBoxSkin::AnimatedTextBoxSkin(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeGeometry>& geom)
    : AnimatedObject(box, geom)
    , m_cursorOffsetX(0)
    , m_cursorPos(0)
    , m_drawCursor(false)
    , m_loaded(false)
    , m_cursor(std::make_shared<TextBoxCursor>())
    , m_text(std::make_shared<EditableLabel>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue())))
{}

AnimatedTextBoxSkin::AnimatedTextBoxSkin(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<EditableLabel>& text,
    const std::shared_ptr<TextBoxCursor>& cursor,
    const std::shared_ptr<IRelativeGeometry>& geom)
    : AnimatedObject(box, geom)
    , m_shiftPeriod(DEFAULT_SHIFT_PERIOD)
    , m_cursorPos(0)
    , m_drawCursor(false)
    , m_loaded(false)
    , m_cursor(cursor ? cursor : std::make_shared<TextBoxCursor>())
    , m_text(text ? text : std::make_shared<EditableLabel>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue())))
{}

void AnimatedTextBoxSkin::setShiftPeriod(Time value)
{
    if (value == 0)
        m_shiftPeriod = DEFAULT_SHIFT_PERIOD;
    else
        m_shiftPeriod = value;
}

void AnimatedTextBoxSkin::setSelection(size_t startIndex, size_t endIndex)
{
    if (startIndex == endIndex) {
        m_cursorPos = startIndex;
        m_drawCursor = true;
    } else {
        m_drawCursor = false;
    }
    m_text->setSelection(startIndex, endIndex);
}

void AnimatedTextBoxSkin::loadResources()
{
    if (!m_loaded) {
        AnimatedObject::loadResources();
        m_loaded = true;
    }

    m_text->loadResources();
    BoundingBox charBox = m_text->textGeometry().at(m_cursorPos).position;
    m_cursor->setX(charBox.bottomLeft.x + m_cursorOffsetX);
    m_cursor->setYRange(charBox.bottomLeft.y, charBox.topRight.y);
    m_cursor->loadResources();
}

void AnimatedTextBoxSkin::drawAt(const Transform2& position) const
{
    AnimatedObject::drawAt(position);
    m_text->draw(position);
    if (m_selectionState == SelectionState::Selected && m_drawCursor)
        m_cursor->draw(position);
}

void AnimatedTextBoxSkin::setBox(const BoundingBox& allowedBox)
{
    AnimatedObject::setBox(allowedBox);
    m_text->setBox(m_box->get());
}

void AnimatedTextBoxSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    AnimatedObject::registerObject(builder);
    builder->registerObject("label", m_text.get());
    builder->registerObject(m_cursor.get());
}

void AnimatedTextBoxSkin::serialize(Serializer& s) const
{
	s << "shiftPeriod" << m_shiftPeriod;
    AnimatedObject::serialize(s);
    s << "label" << m_text << "cursor" << m_cursor;
}

std::unique_ptr<IObject> deserializeAnimatedTextBoxSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<EditableLabel>, label);
    DESERIALIZE(std::shared_ptr<TextBoxCursor>, cursor);
    DESERIALIZE(std::shared_ptr<IRelativeGeometry>, geometry);
    DESERIALIZE_OPT(Time, shiftPeriod, 0);

    std::unique_ptr<AnimatedTextBoxSkin> result(new AnimatedTextBoxSkin(box, label, cursor, geometry));
    result->setShiftPeriod(shiftPeriod);
    deserializeAnimatedObjectElements(deserializer, result.get());
    return std::move(result);
}

REGISTER_CLASS(AnimatedTextBoxSkin);

} }
