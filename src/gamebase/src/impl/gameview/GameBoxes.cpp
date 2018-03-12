/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */
#include "stdafx.h"
#include "GameBoxes.h"
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

RelativeGameBox::RelativeGameBox(
    CoordSystem coordSystem,
    const std::shared_ptr<IRelativeBox>& relativeBox)
    : m_coordSystem(coordSystem)
    , m_relativeBox(relativeBox)
{}

boost::optional<BoundingBox> RelativeGameBox::box() const
{
    return m_box;
}

void RelativeGameBox::setViewBox(const BoundingBox& viewBox)
{
    m_relativeBox->setParentBox(viewBox);
    auto relBox = m_relativeBox->get();
    m_box = m_coordSystem == Center
        ? BoundingBox(relBox.width(), relBox.height())
        : BoundingBox(Vec2(0, 0), Vec2(relBox.width(), relBox.height()));
}

void RelativeGameBox::serialize(Serializer& s) const
{
    s << "coordSystem" << m_coordSystem << "box" << m_relativeBox;
}

FixedGameBox::FixedGameBox(const BoundingBox& box)
    : m_box(box)
{}

boost::optional<BoundingBox> FixedGameBox::box() const
{
    return m_box;
}

void FixedGameBox::serialize(Serializer& s) const
{
    s << "left" << m_box.left() << "bottom" << m_box.bottom()
        << "right" << m_box.right() << "top" << m_box.top();
}

std::unique_ptr<IObject> deserializeInfiniteGameBox(Deserializer& deserializer)
{
    return std::make_unique<InfiniteGameBox>();
}

std::unique_ptr<IObject> deserializeRelativeGameBox(Deserializer& deserializer)
{
    DESERIALIZE(RelativeGameBox::CoordSystem, coordSystem);
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    return std::make_unique<RelativeGameBox>(coordSystem, box);
}

std::unique_ptr<IObject> deserializeFixedGameBox(Deserializer& deserializer)
{
    DESERIALIZE(float, left);
    DESERIALIZE(float, bottom);
    DESERIALIZE(float, right);
    DESERIALIZE(float, top);
    BoundingBox box(Vec2(left, bottom), Vec2(right, top));
    return std::make_unique<FixedGameBox>(box);
}

REGISTER_CLASS(InfiniteGameBox);
REGISTER_CLASS(RelativeGameBox);
REGISTER_CLASS(FixedGameBox);

} }
