#include <stdafx.h>
#include <gamebase/engine/StaticFilledRect.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

void StaticFilledRect::serialize(Serializer& s) const
{
    s << "box" << m_box << "color" << m_color;
}

IObject* deserializeStaticFilledRect(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(Color, color);
    auto* result = new StaticFilledRect(box);
    result->setColor(color);
    return result;
}

REGISTER_CLASS(StaticFilledRect);

}
