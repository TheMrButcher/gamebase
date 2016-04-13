#include <stdafx.h>
#include <gamebase/engine/HorizontalLayoutSkin.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/PixelBox.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {
    
std::shared_ptr<IRelativeOffset>
HorizontalLayoutSkin::createOffset(size_t index) const
{
    return std::make_shared<AligningOffset>(HorAlign::Left, m_align,
        index == 0 ? RelativeValue(RelType::Pixels, 0) : m_padding,
        RelativeValue(RelType::Pixels, 0));
}

void HorizontalLayoutSkin::setExtent(const BoundingBox& box)
{
    if (m_adjustSize)
        m_curBox = box;
}

void HorizontalLayoutSkin::setFixedBox(float width, float height)
{
    m_box = std::make_shared<PixelBox>(width, height);
}

void HorizontalLayoutSkin::serialize(Serializer& s) const
{
    s << "box" << m_box << "padding" << m_padding << "adjustSize" << m_adjustSize << "align" << m_align;
}

std::unique_ptr<IObject> deserializeHorizontalLayoutSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(RelativeValue, padding);
    DESERIALIZE(bool, adjustSize);
    DESERIALIZE(VertAlign::Enum, align);
    std::unique_ptr<HorizontalLayoutSkin> result(new HorizontalLayoutSkin(box));
    result->setPadding(padding);
    result->setAdjustSize(adjustSize);
    result->setAlign(align);
    return std::move(result);
}

REGISTER_CLASS(HorizontalLayoutSkin);

}
