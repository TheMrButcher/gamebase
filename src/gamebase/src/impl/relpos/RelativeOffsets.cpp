/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/relpos/FixedOffset.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void AligningOffset::serialize(Serializer& s) const
{
    s << "horAlign" << m_horAlign << "vertAlign" << m_vertAlign
        << "horOffset" << m_horOffset << "vertOffset" << m_vertOffset;
}

void FixedOffset::serialize(Serializer& s) const
{
    s << "offset" << m_value;
}

std::unique_ptr<IObject> deserializeAligningOffset(Deserializer& deserializer)
{
    DESERIALIZE(HorAlign::Enum, horAlign);
    DESERIALIZE(VertAlign::Enum, vertAlign);
    DESERIALIZE(RelativeValue, horOffset);
    DESERIALIZE(RelativeValue, vertOffset);
    return std::unique_ptr<IObject>(new AligningOffset(
        horAlign, vertAlign, horOffset, vertOffset));
}

std::unique_ptr<IObject> deserializeFixedOffset(Deserializer& deserializer)
{
    DESERIALIZE(Vec2, offset);
    return std::unique_ptr<IObject>(new FixedOffset(offset));
}

REGISTER_CLASS(AligningOffset);
REGISTER_CLASS(FixedOffset);

} }
