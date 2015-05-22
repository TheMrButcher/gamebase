#include <stdafx.h>
#include <gamebase/engine/TextEditCursor.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

TextEditCursor::TextEditCursor()
    : OffsettedPosition(std::make_shared<FixedOffset>())
    , Drawable(this)
    , m_width(1.0f)
    , m_cursorPeriod(TimeState::Real, 1000)
{
    m_rect.setColor(Color(0, 0, 0));
}

void TextEditCursor::serialize(Serializer& s) const
{
    s << "width" << m_width << "color" << m_rect.color() << "period" << m_cursorPeriod;
}

IObject* deserializeTextEditCursor(Deserializer& deserializer)
{
    DESERIALIZE(float, width);
    DESERIALIZE(Color, color);
    DESERIALIZE(TypedTime, period);
    auto* result = new TextEditCursor();
    result->setWidth(width);
    result->setColor(color);
    result->setPeriod(period);
    return result;
}

REGISTER_CLASS(TextEditCursor);

bool TextEditCursor::needToDraw() const
{
    if (m_cursorPeriod.value == 0)
        return true;
    Time curTimeInPeriod = TimeState::time(m_cursorPeriod.type).value % m_cursorPeriod.value;
    // is it first half of period?
    return curTimeInPeriod * 2 < m_cursorPeriod.value;
}

}
