#include <stdafx.h>
#include <gamebase/engine/TransparentLinearLayoutSkin.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {
    
std::shared_ptr<IRelativeOffset>
TransparentLinearLayoutSkin::createOffset(size_t index) const
{
    float offset = index == 0 ? 0.0f : m_padding;
    float horOffset = offset;
    float vertOffset = 0.0f;
    if (m_direction == Direction::Vertical) {
        vertOffset = -offset;
        horOffset = 0.0f;
    }
    return std::make_shared<AligningOffset>(HorAlign::Left, VertAlign::Top,
        RelativeValue(RelType::Pixels, horOffset),
        RelativeValue(RelType::Pixels, vertOffset));
}

void TransparentLinearLayoutSkin::setSize(float width, float height)
{
    if (m_adjustSize) {
        m_curBox.topRight.x = m_curBox.bottomLeft.x + width;
        m_curBox.bottomLeft.y = m_curBox.topRight.y - height;
    }
}

void TransparentLinearLayoutSkin::serialize(Serializer& s) const
{
    s << "box" << m_box << "direction" << m_direction
        << "padding" << m_padding << "adjustSize" << m_adjustSize;
}

std::unique_ptr<IObject> deserializeTransparentLinearLayoutSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(Direction::Enum, direction);
    DESERIALIZE(float, padding);
    DESERIALIZE(bool, adjustSize);
    std::unique_ptr<TransparentLinearLayoutSkin> result(
        new TransparentLinearLayoutSkin(box, direction));
    result->setPadding(padding);
    result->setAdjustSize(adjustSize);
    return std::move(result);
}

REGISTER_CLASS(TransparentLinearLayoutSkin);

}


