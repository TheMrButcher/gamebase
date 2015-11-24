#include <stdafx.h>
#include <gamebase/engine/OffsettedPositionElement.h>
#include <gamebase/engine/RotatedPositionElement.h>
#include <gamebase/engine/TransformedPositionElement.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

void OffsettedPositionElement::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerVec2("offset", &m_pos.offset);
}

void OffsettedPositionElement::serialize(Serializer& s) const
{
    s << "offset" << m_pos.offset;
}

std::unique_ptr<IObject> deserializeOffsettedPositionElement(Deserializer& deserializer)
{
    DESERIALIZE(Vec2, offset);
    return std::unique_ptr<OffsettedPositionElement>(new OffsettedPositionElement(offset));
}

REGISTER_CLASS(OffsettedPositionElement);

void RotatedPositionElement::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerVec2("offset", &m_pos.offset);
    builder->registerProperty<float>(
        "angle", &m_angle, std::bind(&RotatedPositionElement::setAngle, this, std::placeholders::_1));
}

void RotatedPositionElement::serialize(Serializer& s) const
{
    s << "offset" << m_pos.offset << "angle" << m_angle;
}

std::unique_ptr<IObject> deserializeRotatedPositionElement(Deserializer& deserializer)
{
    DESERIALIZE(Vec2, offset);
    DESERIALIZE(float, angle);
    return std::unique_ptr<RotatedPositionElement>(new RotatedPositionElement(angle, offset));
}

REGISTER_CLASS(RotatedPositionElement);

void TransformedPositionElement::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerVec2("offset", &m_pos.offset);
    builder->registerProperty<float>(
        "angle", &m_angle, std::bind(&RotatedPositionElement::setAngle, this, std::placeholders::_1));
    builder->registerProperty<float>(
        "scaleX", &m_scaleX, std::bind(&RotatedPositionElement::setScaleX, this, std::placeholders::_1));
    builder->registerProperty<float>(
        "scaleY", &m_scaleY, std::bind(&RotatedPositionElement::setScaleY, this, std::placeholders::_1));
}

void TransformedPositionElement::serialize(Serializer& s) const
{
    s << "offset" << m_pos.offset << "angle" << m_angle << "scaleX" << m_scaleX << "scaleY" << m_scaleY;
}

std::unique_ptr<IObject> deserializeTransformedPositionElement(Deserializer& deserializer)
{
    DESERIALIZE(Vec2, offset);
    DESERIALIZE(float, angle);
    DESERIALIZE(float, scaleX);
    DESERIALIZE(float, scaleY);
    return std::unique_ptr<TransformedPositionElement>(new TransformedPositionElement(scaleX, scaleY, angle, offset));
}

REGISTER_CLASS(TransformedPositionElement);

}
