#include <stdafx.h>
#include <gamebase/engine/SquareBox.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/engine/FixedBox.h>
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

IObject* deserializeSquareBox(Deserializer&)
{
    return new SquareBox();
}

IObject* deserializeRelativeBox(Deserializer& deserializer)
{
    DESERIALIZE(RelativeValue, width);
    DESERIALIZE(RelativeValue, height);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, offset);
    return new RelativeBox(width, height, offset);
}

IObject* deserializeFixedBox(Deserializer& deserializer)
{
    DESERIALIZE(BoundingBox, box);
    return new FixedBox(box);
}

REGISTER_CLASS(SquareBox);
REGISTER_CLASS(RelativeBox);
REGISTER_CLASS(FixedBox);

}
