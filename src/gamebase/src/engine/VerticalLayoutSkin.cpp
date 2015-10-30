#include <stdafx.h>
#include <gamebase/engine/VerticalLayoutSkin.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {
    
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

}
