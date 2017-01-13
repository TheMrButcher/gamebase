/**
 * Copyright (c) 2016 Slavnejshev Filipp
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

/*namespace {
void serializeOffsets(
    const RelativeValueInUI& horOffset,
    const RelativeValueInUI& vertOffset,
    impl::Serializer& s)
{
    s << "horOffset" << horOffset << "vertOffset" << vertOffset;
}

void serializeOffsets(
	const impl::RelativeValue& horOffset,
    const impl::RelativeValue& vertOffset,
    impl::Serializer& s)
{
    serializeOffsets(RelativeValueInUI(horOffset), RelativeValueInUI(vertOffset), s);
}

void serializeBoxOffset(
    const impl::IRelativeOffset* offset, impl::Serializer& s)
{
    if (offset) {
        if (auto aligningOffset = dynamic_cast<const impl::AligningOffset*>(offset)) {
            if (aligningOffset->horAlign() != impl::HorAlign::None
                || aligningOffset->vertAlign() != impl::VertAlign::None)
                std::cerr << "Warning: offset of box shouldn't contain any alignment" << std::endl;
			serializeOffsets(
				RelativeValueInUI(aligningOffset->horOffset()), 
				RelativeValueInUI(aligningOffset->vertOffset()),
				s);
			return;
        } else if (auto fixedOffset = dynamic_cast<const impl::FixedOffset*>(offset)) {
            Vec2 v = fixedOffset->get();
			serializeOffsets(pixels(v.x), pixels(v.y), s);
			return;
        } else {
            std::cerr << "Warning: unknown type of offset! Replaced with zero" << std::endl;
        }
    }
    serializeOffsets(zeroValue(), zeroValue(), s);
}
}

void serializeRelativeBoxAsComplex(
    const impl::IObject* obj, impl::Serializer& s)
{
    if (auto box = dynamic_cast<const impl::RelativeBox*>(obj)) {
        s   << "widthSource" << impl::BoxSize::Width
            << "heightSource" << impl::BoxSize::Height
            << "width" << RelativeValueInUI(box->widthValue())
            << "height" << RelativeValueInUI(box->heightValue());
        serializeBoxOffset(box->offset().get(), s);
    } else {
        THROW_EX() << "Serializer expected RelativeBox as input";
    }
}

void serializeOffsettedBoxAsComplex(
    const impl::IObject* obj, impl::Serializer& s)
{
    std::cerr << "Warning: detected deprecated OffsettedBox" << std::endl;
    if (auto box = dynamic_cast<const impl::OffsettedBox*>(obj)) {
        s   << "widthSource" << impl::BoxSize::Width
            << "heightSource" << impl::BoxSize::Height
            << "width" << identic()
            << "height" << identic();
        serializeBoxOffset(box->offset().get(), s);
    } else {
        THROW_EX() << "Serializer expected OffsettedBox as input";
    }
}

void serializeSquareBoxAsComplex(
    const impl::IObject* obj, impl::Serializer& s)
{
    if (auto box = dynamic_cast<const impl::SquareBox*>(obj)) {
        s   << "widthSource" << impl::BoxSize::Min
            << "heightSource" << impl::BoxSize::Min
            << "width" << identic()
            << "height" << identic();
        serializeOffsets(zeroValue(), zeroValue(), s);
    } else {
        THROW_EX() << "Serializer expected SquareBox as input";
    }
}

void serializeFixedBoxAsComplex(
    const impl::IObject* obj, impl::Serializer& s)
{
    if (auto box = dynamic_cast<const impl::FixedBox*>(obj)) {
        auto bb = box->count(Box());
        s   << "widthSource" << impl::BoxSize::Width
            << "heightSource" << impl::BoxSize::Height
            << "width" << pixels(bb.width())
            << "height" << pixels(bb.height());
        Vec2 offset = bb.center();
        serializeOffsets(pixels(offset.x), pixels(offset.y), s);
    } else {
        THROW_EX() << "Serializer expected FixedBox as input";
    }
}

void serializePixelBoxAsComplex(
    const impl::IObject* obj, impl::Serializer& s)
{
    if (auto box = dynamic_cast<const impl::PixelBox*>(obj)) {
        auto bb = box->count(Box());
        s   << "widthSource" << impl::BoxSize::Width
            << "heightSource" << impl::BoxSize::Height
            << "width" << pixels(bb.width())
            << "height" << pixels(bb.height());
        serializeOffsets(zeroValue(), zeroValue(), s);
    } else {
        THROW_EX() << "Serializer expected PixelBox as input";
    }
}

void serializeComplexBoxForUI(const impl::IObject* obj, impl::Serializer& s)
{
	if (auto box = dynamic_cast<const impl::ComplexBox*>(obj)) {
		s   << "widthSource" << box->widthSource()
			<< "heightSource" << box->heightSource()
            << "width" << RelativeValueInUI(box->widthValue())
            << "height" << RelativeValueInUI(box->heightValue());
		serializeOffsets(box->horOffset(), box->vertOffset(), s);
    } else {
        THROW_EX() << "Serializer expected ComplexBox as input";
    }
}*/

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
        Box bb(width->valueFloat(), height->valueFloat(),
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
