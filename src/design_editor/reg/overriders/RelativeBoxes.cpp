/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "RelativeBoxes.h"
#include "RelativeValueInUI.h"
#include <reg/tools.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/relpos/FixedOffset.h>

namespace gamebase { namespace editor {

void ComplexBoxInUI::serialize(impl::Serializer& s) const
{
	s   << "widthSource" << widthSource() << "heightSource" << heightSource()
        << "width" << m_widthValueInUI << "height" << m_heightValueInUI
        << "horOffset" << m_horOffsetInUI << "vertOffset" << m_vertOffsetInUI;
}

void ComplexBoxInUI::setOffset(impl::IRelativeOffset* offset)
{
	if (offset) {
        if (auto aligningOffset = dynamic_cast<const impl::AligningOffset*>(offset)) {
            if (aligningOffset->horAlign() != impl::HorAlign::None
                || aligningOffset->vertAlign() != impl::VertAlign::None)
                std::cerr << "Warning: offset of box shouldn't contain any alignment" << std::endl;
			setOffset(
				aligningOffset->horOffset(), 
				aligningOffset->vertOffset());
			return;
        } else if (auto fixedOffset = dynamic_cast<const impl::FixedOffset*>(offset)) {
            Vec2 v = fixedOffset->get();
			setOffset(pixels(v.x), pixels(v.y));
			return;
        } else {
            std::cerr << "Warning: unknown type of offset! Replaced with zero" << std::endl;
        }
    }
    setOffset(zeroValue(), zeroValue());
}

std::unique_ptr<impl::IObject> deserializeSquareBox(impl::Deserializer&)
{
    return std::unique_ptr<impl::IObject>(new ComplexBoxInUI(
		identic(), identic(),
		impl::BoxSize::Min, impl::BoxSize::Min));
}

std::unique_ptr<impl::IObject> deserializeRelativeBox(impl::Deserializer& deserializer)
{
	using namespace impl;
    DESERIALIZE(RelativeValue, width);
    DESERIALIZE(RelativeValue, height);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, offset);
    std::unique_ptr<ComplexBoxInUI> result(new ComplexBoxInUI(width, height));
	result->setOffset(offset.get());
	return std::move(result);
}

std::unique_ptr<impl::IObject> deserializeFixedBox(impl::Deserializer& deserializer)
{
	using namespace impl;
    DESERIALIZE(BoundingBox, box);
    return std::unique_ptr<IObject>(new ComplexBoxInUI(
		pixels(box.width()), pixels(box.height()),
		BoxSize::Width, BoxSize::Height,
		pixels(box.center().x), pixels(box.center().y)));
}

std::unique_ptr<impl::IObject> deserializeOffsettedBox(impl::Deserializer& deserializer)
{
	using namespace impl;
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, offset);
	std::unique_ptr<ComplexBoxInUI> result(new ComplexBoxInUI(identic(), identic()));
	result->setOffset(offset.get());
	return std::move(result);
}

std::unique_ptr<impl::IObject> deserializePixelBox(impl::Deserializer& deserializer)
{
	using namespace impl;
    DESERIALIZE(float, width);
    DESERIALIZE(float, height);
    return std::unique_ptr<IObject>(new ComplexBoxInUI(
		pixels(width), pixels(height)));
}

std::unique_ptr<impl::IObject> deserializeComplexBox(impl::Deserializer& deserializer)
{
	using namespace impl;
    DESERIALIZE(RelativeValue, width);
    DESERIALIZE(RelativeValue, height);
    DESERIALIZE(BoxSize::Type, widthSource);
    DESERIALIZE(BoxSize::Type, heightSource);
    DESERIALIZE(RelativeValue, horOffset);
    DESERIALIZE(RelativeValue, vertOffset);
    return std::unique_ptr<IObject>(new ComplexBoxInUI(
        width, height, widthSource, heightSource, horOffset, vertOffset));
}

std::unique_ptr<impl::IObject> deserializeComplexBoxInUI(impl::Deserializer& deserializer)
{
    using namespace impl;
    DESERIALIZE(std::shared_ptr<RelativeValueInUI>, width);
    DESERIALIZE(std::shared_ptr<RelativeValueInUI>, height);
    DESERIALIZE(BoxSize::Type, widthSource);
    DESERIALIZE(BoxSize::Type, heightSource);
    DESERIALIZE(std::shared_ptr<RelativeValueInUI>, horOffset);
    DESERIALIZE(std::shared_ptr<RelativeValueInUI>, vertOffset);
    std::unique_ptr<IObject> result;
    if (isZero(*horOffset) && isZero(*vertOffset)) {
        if (width->type() == height->type()) {
            if (widthSource == BoxSize::Min && heightSource == BoxSize::Min && isIdentic(*width))
                result.reset(new SquareBox());
            if (width->type() == RelType::Pixels)
                result.reset(new PixelBox(width->valueFloat(), height->valueFloat()));
        }
    }
    
    if (!result && width->type() == RelType::Pixels && height->type() == RelType::Pixels
        && horOffset->type() == RelType::Pixels && vertOffset->type() == RelType::Pixels) {
        BoundingBox bb(width->valueFloat(), height->valueFloat(),
			Vec2(horOffset->valueFloat(), vertOffset->valueFloat()));
        result.reset(new FixedBox(bb));
    }

    if (!result && widthSource == BoxSize::Width && heightSource == BoxSize::Height) {
        std::shared_ptr<IRelativeOffset> offset;
        if (horOffset->type() == RelType::Pixels && vertOffset->type() == RelType::Pixels) {
            if (horOffset->value() != 0 || vertOffset->value() != 0)
                offset.reset(new FixedOffset(
					horOffset->valueFloat(), vertOffset->valueFloat()));
        } else {
            offset.reset(new AligningOffset(
				HorAlign::None, VertAlign::None,
				horOffset->toRelativeValue(),
				vertOffset->toRelativeValue()));
        }
		result.reset(new RelativeBox(
			width->toRelativeValue(), height->toRelativeValue(), offset));
    }

    if (!result)
        result.reset(new ComplexBox(
			width->toRelativeValue(), height->toRelativeValue(),
			widthSource, heightSource,
			horOffset->toRelativeValue(), vertOffset->toRelativeValue()));
    return std::move(result);
}

REGISTER_CLASS(ComplexBoxInUI);

} }
