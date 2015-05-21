#include <stdafx.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

void StaticLabel::serialize(Serializer& s) const
{
    s << "box" << m_box << "color" << m_color << "properties" << m_alignProps << "text" << m_text;
}

IObject* deserializeStaticLabel(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(Color, color);
    DESERIALIZE(AlignProperties, properties);
    DESERIALIZE(std::string, text);
    auto* result = new StaticLabel(box);
    result->setColor(color);
    result->setAlignProperties(properties);
    result->setText(text);
    return result;
}

REGISTER_CLASS(StaticLabel);

}
