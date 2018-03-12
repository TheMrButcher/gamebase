/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/tools/TextBoxCursor.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

TextBoxCursor::TextBoxCursor()
    : OffsettedPosition(std::make_shared<FixedOffset>())
    , Drawable(this)
    , m_width(1.0f)
    , m_cursorPeriod(TimeState::Real, 1000)
{
    m_rect.setColor(GLColor(0, 0, 0));
}

void TextBoxCursor::serialize(Serializer& s) const
{
    s << "width" << m_width << "color" << m_rect.color() << "period" << m_cursorPeriod;
}

std::unique_ptr<IObject> deserializeTextBoxCursor(Deserializer& deserializer)
{
    DESERIALIZE(float, width);
    DESERIALIZE(GLColor, color);
    DESERIALIZE(TypedTime, period);
    std::unique_ptr<TextBoxCursor> result(new TextBoxCursor());
    result->setWidth(width);
    result->setColor(color);
    result->setPeriod(period);
    return std::move(result);
}

REGISTER_CLASS(TextBoxCursor);

bool TextBoxCursor::needToDraw() const
{
    if (m_cursorPeriod.value == 0)
        return true;
    Time curTimeInPeriod = TimeState::time(m_cursorPeriod.type).value % m_cursorPeriod.value;
    // is it first half of period?
    return curTimeInPeriod * 2 < m_cursorPeriod.value;
}

} }
