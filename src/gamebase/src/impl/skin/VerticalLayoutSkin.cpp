/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/VerticalLayoutSkin.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {
    
std::shared_ptr<IRelativeOffset>
VerticalLayoutSkin::createOffset(size_t index) const
{
    return std::make_shared<AligningOffset>(m_align, VertAlign::Top,
        RelativeValue(RelType::Pixels, 0),
        index == 0 ? RelativeValue(RelType::Pixels, 0) : m_padding);
}

void VerticalLayoutSkin::setExtent(const BoundingBox& box)
{
    if (m_adjustSize)
        m_curBox = box;
}

void VerticalLayoutSkin::setFixedBox(float width, float height)
{
    auto box = std::make_shared<FixedBox>(width, height);
    if (m_box->isValid())
        box->checkInited();
    m_box = box;
}

void VerticalLayoutSkin::serialize(Serializer& s) const
{
    s << "box" << m_box << "padding" << m_padding << "adjustSize" << m_adjustSize << "align" << m_align;
}

std::unique_ptr<IObject> deserializeVerticalLayoutSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(RelativeValue, padding);
    DESERIALIZE(bool, adjustSize);
    DESERIALIZE(HorAlign::Enum, align);
    std::unique_ptr<VerticalLayoutSkin> result(new VerticalLayoutSkin(box));
    result->setPadding(padding);
    result->setAdjustSize(adjustSize);
    result->setAlign(align);
    return std::move(result);
}

REGISTER_CLASS(VerticalLayoutSkin);

} }
