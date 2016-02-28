#include <stdafx.h>
#include <gamebase/engine/SquareBox.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/engine/OffsettedBox.h>
#include <gamebase/engine/PixelBox.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

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

REGISTER_CLASS(SquareBox);
REGISTER_CLASS(RelativeBox);
REGISTER_CLASS(FixedBox);
REGISTER_CLASS(OffsettedBox);
REGISTER_CLASS(PixelBox);

}
