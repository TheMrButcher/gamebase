/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "RelativeBoxes.h"
#include <reg/tools.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/relpos/FixedOffset.h>

namespace gamebase { namespace editor {

namespace {
void serializeOffsets(
    const impl::RelativeValue& horOffset,
    const impl::RelativeValue& vertOffset,
    impl::Serializer& s)
{
    s << "horOffset" << horOffset << "vertOffset" << vertOffset;
}
}

void serializeBoxOffset(
    const impl::IRelativeOffset* offset, impl::Serializer& s)
{
    auto horOffset = zeroValue();
    auto vertOffset = zeroValue();
    if (offset) {
        if (auto aligningOffset = dynamic_cast<const impl::AligningOffset*>(offset)) {
            if (aligningOffset->horAlign() != impl::HorAlign::None
                || aligningOffset->vertAlign() != impl::VertAlign::None)
                std::cerr << "Warning: offset of box shouldn't contain any alignment" << std::endl;
            horOffset = aligningOffset->horOffset();
            vertOffset = aligningOffset->vertOffset();
        } else if (auto fixedOffset = dynamic_cast<const impl::FixedOffset*>(offset)) {
            Vec2 v = fixedOffset->get();
            horOffset = pixels(v.x);
            vertOffset = pixels(v.y);
        } else {
            std::cerr << "Warning: unknown type of offset! Replaced with zero" << std::endl;
        }
    }
    serializeOffsets(horOffset, vertOffset, s);
}

void serializeRelativeBoxAsComplex(
    const impl::IObject* obj, impl::Serializer& s)
{
    if (auto box = dynamic_cast<const impl::RelativeBox*>(obj)) {
        s   << "widthSource" << impl::BoxSize::Width
            << "heightSource" << impl::BoxSize::Height
            << "width" << box->widthValue()
            << "height" << box->heightValue();
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

std::unique_ptr<impl::IObject> deserializeBox(impl::Deserializer& deserializer)
{
    using namespace impl;
    DESERIALIZE(RelativeValue, width);
    DESERIALIZE(RelativeValue, height);
    DESERIALIZE(BoxSize::Type, widthSource);
    DESERIALIZE(BoxSize::Type, heightSource);
    DESERIALIZE(RelativeValue, horOffset);
    DESERIALIZE(RelativeValue, vertOffset);
    std::unique_ptr<IObject> result;
    if (isZero(horOffset) && isZero(vertOffset)) {
        if (width.type() == height.type()) {
            if (widthSource == BoxSize::Min && heightSource == BoxSize::Min && isIdentic(width))
                result.reset(new SquareBox());
            if (width.type() == RelType::Pixels)
                result.reset(new PixelBox(width.value(), height.value()));
        }
    }
    
    if (!result && width.type() == RelType::Pixels && height.type() == RelType::Pixels
        && horOffset.type() == RelType::Pixels && vertOffset.type() == RelType::Pixels) {
        Box bb(width.value(), height.value(), Vec2(horOffset.value(), vertOffset.value()));
        result.reset(new FixedBox(bb));
    }

    if (!result && widthSource == BoxSize::Width && heightSource == BoxSize::Height) {
        std::shared_ptr<IRelativeOffset> offset;
        if (horOffset.type() == RelType::Pixels && vertOffset.type() == RelType::Pixels) {
            if (horOffset.value() != 0 || vertOffset.value() != 0)
                offset.reset(new FixedOffset(horOffset.value(), vertOffset.value()));
        } else {
            offset.reset(new AligningOffset(HorAlign::None, VertAlign::None, horOffset, vertOffset));
        }
        result.reset(new RelativeBox(width, height, offset));
    }

    if (!result)
        result.reset(new ComplexBox(width, height, widthSource, heightSource, horOffset, vertOffset));
    return std::move(result);
}

} }
