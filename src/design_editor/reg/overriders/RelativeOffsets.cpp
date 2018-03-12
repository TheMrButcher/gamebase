/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "RelativeOffsets.h"
#include <reg/tools.h>

namespace gamebase { namespace editor {

void AligningOffsetInUI::serialize(impl::Serializer& s) const
{
	s   << "horAlign" << horAlign()
		<< "vertAlign" << vertAlign()
		<< "horOffset" << m_horOffsetInUI
		<< "vertOffset" << m_vertOffsetInUI;
}

std::unique_ptr<impl::IObject> deserializeFixedOffset(impl::Deserializer& deserializer)
{
    DESERIALIZE(Vec2, offset);
    return std::unique_ptr<impl::IObject>(new AligningOffsetInUI(
		impl::HorAlign::None, impl::VertAlign::None, 
		pixels(offset.x),
		pixels(offset.y)));
}

std::unique_ptr<impl::IObject> deserializeAligningOffset(impl::Deserializer& deserializer)
{
    using namespace impl;
	DESERIALIZE(HorAlign::Enum, horAlign);
    DESERIALIZE(VertAlign::Enum, vertAlign);
    DESERIALIZE(RelativeValue, horOffset);
    DESERIALIZE(RelativeValue, vertOffset);
    return std::unique_ptr<IObject>(new AligningOffsetInUI(
        horAlign, vertAlign, horOffset, vertOffset));
}

/*void serializeFixedOffsetAsAligning(
    const impl::IObject* obj, impl::Serializer& s)
{
    if (auto offset = dynamic_cast<const impl::FixedOffset*>(obj)) {
        Vec2 v = offset->get();
        s   << "horAlign" << impl::HorAlign::None
            << "vertAlign" << impl::VertAlign::None
            << "horOffset" << pixels(v.x)
            << "vertOffset" << pixels(v.y);
    } else {
        THROW_EX() << "Serializer expected FixedOffset as input";
    }
}

void serializeAligningOffsetForUI(const impl::IObject* obj, impl::Serializer& s)
{
	if (auto offset = dynamic_cast<const impl::AligningOffset*>(obj)) {
		s   << "horAlign" << offset->horAlign()
			<< "vertAlign" << offset->vertAlign()
			<< "horOffset" << RelativeValueInUI(offset->horOffset())
			<< "vertOffset" << RelativeValueInUI(offset->vertOffset());
    } else {
        THROW_EX() << "Serializer expected AligningOffset as input";
    }
}*/

std::unique_ptr<impl::IObject> deserializeAligningOffsetInUI(impl::Deserializer& deserializer)
{
    using namespace impl;
    DESERIALIZE(HorAlign::Enum, horAlign);
    DESERIALIZE(VertAlign::Enum, vertAlign);
    DESERIALIZE(std::shared_ptr<RelativeValueInUI>, horOffset);
    DESERIALIZE(std::shared_ptr<RelativeValueInUI>, vertOffset);
    std::unique_ptr<IObject> result;
    if (horAlign == HorAlign::None && vertAlign == VertAlign::None
        && horOffset->type() == RelType::Pixels && vertOffset->type() == RelType::Pixels) {
        result.reset(new FixedOffset(horOffset->valueFloat(), vertOffset->valueFloat()));
    } else {
		result.reset(new AligningOffset(
			horAlign, vertAlign,
			horOffset->toRelativeValue(),
			vertOffset->toRelativeValue()));
    }
    return std::move(result);
}

REGISTER_CLASS(AligningOffsetInUI);

} }
