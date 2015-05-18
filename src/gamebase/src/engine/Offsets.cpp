#include <stdafx.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/FixedOffset.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

void AligningOffset::serialize(Serializer& s) const
{
    s << "horAlign" << m_horAlign << "vertAlign" << m_vertAlign;
}

void FixedOffset::serialize(Serializer& s) const
{
    s << "offset" << m_value;
}

IObject* deserializeAligningOffset(Deserializer& deserializer)
{
    DESERIALIZE(HorAlign::Enum, horAlign);
    DESERIALIZE(VertAlign::Enum, vertAlign);
    return new AligningOffset(horAlign, vertAlign);
}

IObject* deserializeFixedOffset(Deserializer& deserializer)
{
    DESERIALIZE(Vec2, offset);
    return new FixedOffset(offset);
}

REGISTER_CLASS(AligningOffset);
REGISTER_CLASS(FixedOffset);

}
