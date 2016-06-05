#include <stdafx.h>
#include <gamebase/impl/drawobj/StaticLabel.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void StaticLabel::serialize(Serializer& s) const
{
    s << "box" << m_box << "position" << m_offset << "color" << m_color << "properties" << m_alignProps
        << "text" << m_text << "adjustSize" << m_adjustSize;
}

std::unique_ptr<IObject> deserializeStaticLabel(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(Color, color);
    DESERIALIZE(AlignProperties, properties);
    DESERIALIZE(std::string, text);
    DESERIALIZE(bool, adjustSize);
    std::unique_ptr<StaticLabel> result(new StaticLabel(box, position));
    result->setColor(color);
    result->setAlignProperties(properties);
    result->setText(text);
    result->setAdjustSize(adjustSize);
    return std::move(result);
}

REGISTER_CLASS(StaticLabel);

} }
