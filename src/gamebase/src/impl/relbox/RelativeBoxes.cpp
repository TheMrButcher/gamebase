/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/relbox/SquareBox.h>
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/relbox/OffsettedBox.h>
#include <gamebase/impl/relbox/PixelBox.h>
#include <gamebase/impl/relbox/ComplexBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void RelativeBox::serialize(Serializer& s) const
{
    s << "width" << m_widthValue << "height" << m_heightValue << "offset" << m_offset;
}

void FixedBox::serialize(Serializer& s) const
{
    s << "box" << m_box;
}

void OffsettedBox::serialize(Serializer& s) const
{
    s << "offset" << m_offset;
}

void PixelBox::serialize(Serializer& s) const
{
    s << "width" << m_box.width() << "height" << m_box.height();
}

void ComplexBox::serialize(Serializer& s) const
{
    s << "widthSource" << m_widthSource << "heightSource" << m_heightSource
        << "width" << m_widthValue << "height" << m_heightValue
        << "horOffset" << m_horOffset << "vertOffset" << m_vertOffset;
}

std::unique_ptr<IObject> deserializeSquareBox(Deserializer&)
{
    return std::unique_ptr<IObject>(new SquareBox());
}

std::unique_ptr<IObject> deserializeRelativeBox(Deserializer& deserializer)
{
    DESERIALIZE(RelativeValue, width);
    DESERIALIZE(RelativeValue, height);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, offset);
    return std::unique_ptr<IObject>(new RelativeBox(width, height, offset));
}

std::unique_ptr<IObject> deserializeFixedBox(Deserializer& deserializer)
{
    DESERIALIZE(BoundingBox, box);
    return std::unique_ptr<IObject>(new FixedBox(box));
}

std::unique_ptr<IObject> deserializeOffsettedBox(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, offset);
    return std::unique_ptr<IObject>(new OffsettedBox(offset));
}

std::unique_ptr<IObject> deserializePixelBox(Deserializer& deserializer)
{
    DESERIALIZE(float, width);
    DESERIALIZE(float, height);
    return std::unique_ptr<IObject>(new PixelBox(width, height));
}

std::unique_ptr<IObject> deserializeComplexBox(Deserializer& deserializer)
{
    DESERIALIZE(RelativeValue, width);
    DESERIALIZE(RelativeValue, height);
    DESERIALIZE(BoxSize::Type, widthSource);
    DESERIALIZE(BoxSize::Type, heightSource);
    DESERIALIZE(RelativeValue, horOffset);
    DESERIALIZE(RelativeValue, vertOffset);
    return std::unique_ptr<IObject>(new ComplexBox(
        width, height, widthSource, heightSource, horOffset, vertOffset));
}

REGISTER_CLASS(SquareBox);
REGISTER_CLASS(RelativeBox);
REGISTER_CLASS(FixedBox);
REGISTER_CLASS(OffsettedBox);
REGISTER_CLASS(PixelBox);
REGISTER_CLASS(ComplexBox);

} }
