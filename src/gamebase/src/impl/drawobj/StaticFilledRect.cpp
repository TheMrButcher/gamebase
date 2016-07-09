#include <stdafx.h>
#include <gamebase/impl/drawobj/StaticFilledRect.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void StaticFilledRect::setFixedBox(float width, float height)
{
    auto box = std::make_shared<FixedBox>(width, height);
    if (m_box->isValid())
        box->checkInited();
    m_box = box;
    TextureRect::loadResources();
}

void StaticFilledRect::serialize(Serializer& s) const
{
    s << "box" << m_box << "position" << m_offset << "color" << m_color;
}

std::unique_ptr<IObject> deserializeStaticFilledRect(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(Color, color);
    std::unique_ptr<StaticFilledRect> result(new StaticFilledRect(box, position));
    result->setColor(color);
    return std::move(result);
}

REGISTER_CLASS(StaticFilledRect);

} }
