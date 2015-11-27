#include <stdafx.h>
#include <gamebase/engine/InactiveObjectConstruct.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>
#include <gamebase/engine/OffsettedPositionElement.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

InactiveObjectConstruct::InactiveObjectConstruct(
    const std::shared_ptr<IDrawable>& drawable,
    const std::shared_ptr<PositionElement>& position)
    : Drawable(this)
    , m_drawable(drawable)
    , m_posElement(position ? position : std::make_shared<OffsettedPositionElement>())
{}

void InactiveObjectConstruct::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("skin", m_drawable.get());
    builder->registerObject("position", m_posElement.get());
}

void InactiveObjectConstruct::serialize(Serializer& s) const
{
    s << "skin" << m_drawable << "position" << m_posElement;
}

std::unique_ptr<IObject> deserializeInactiveObjectConstruct(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IDrawable>, skin);
    DESERIALIZE(std::shared_ptr<PositionElement>, position);
    return std::unique_ptr<InactiveObjectConstruct>(new InactiveObjectConstruct(skin, position));
}

REGISTER_CLASS(InactiveObjectConstruct);

}
