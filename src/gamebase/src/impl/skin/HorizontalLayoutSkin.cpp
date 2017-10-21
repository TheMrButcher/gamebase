/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/HorizontalLayoutSkin.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {
    
std::shared_ptr<IRelativeOffset>
HorizontalLayoutSkin::createOffset(size_t index) const
{
    return std::make_shared<AligningOffset>(
        HorAlign::Left, m_align,
        index == 0 ? RelativeValue(RelType::Pixels, 0) : m_padding,
        RelativeValue(RelType::Pixels, 0));
}

void HorizontalLayoutSkin::setExtent(const BoundingBox& box)
{
    m_curBox = adjust(m_curBox, box, m_adjustment);
}

void HorizontalLayoutSkin::setFixedBox(float width, float height)
{
    auto box = std::make_shared<FixedBox>(width, height);
    if (m_box->isValid())
        box->checkInited();
    m_box = box;
}

void HorizontalLayoutSkin::serialize(Serializer& s) const
{
    s << "box" << m_box << "padding" << m_padding << "adjustment" << m_adjustment << "align" << m_align;
}

std::unique_ptr<IObject> deserializeHorizontalLayoutSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(RelativeValue, padding);
    DESERIALIZE(VertAlign::Enum, align);

    Adjustment::Enum adjustment;
    if (deserializer.hasMember("adjustSize")) {
        DESERIALIZE(bool, adjustSize);
        adjustment = adjustSize ? Adjustment::ToFitContent : Adjustment::None;
    } else {
        deserializer >> "adjustment" >> adjustment;
    }

    std::unique_ptr<HorizontalLayoutSkin> result(new HorizontalLayoutSkin(box));
    result->setPadding(padding);
    result->setAdjustment(adjustment);
    result->setAlign(align);
    return std::move(result);
}

REGISTER_CLASS(HorizontalLayoutSkin);

} }
