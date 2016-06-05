#include <stdafx.h>
#include <gamebase/impl/gameobj/OffsettedPositionElement.h>
#include <gamebase/impl/gameobj/RotatedPositionElement.h>
#include <gamebase/impl/gameobj/TransformedPositionElement.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void OffsettedPositionElement::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerVec2("offset", &m_pos.offset);
    builder->registerProperty("x", &m_pos.offset.x);
    builder->registerProperty("y", &m_pos.offset.y);
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
    builder->registerProperty("x", &m_pos.offset.x);
    builder->registerProperty("y", &m_pos.offset.y);
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
    builder->registerProperty("x", &m_pos.offset.x);
    builder->registerProperty("y", &m_pos.offset.y);
    builder->registerProperty<float>("angle", &m_angle,
        std::bind(&TransformedPositionElement::setAngle, this, std::placeholders::_1));
    builder->registerProperty<float>(
        "sx", &m_scaleX, std::bind(&TransformedPositionElement::setScaleX, this, std::placeholders::_1));
    builder->registerProperty<float>(
        "sy", &m_scaleY, std::bind(&TransformedPositionElement::setScaleY, this, std::placeholders::_1));
    builder->registerProperty<float>(
        "scaleX", &m_scaleX, std::bind(&TransformedPositionElement::setScaleX, this, std::placeholders::_1));
    builder->registerProperty<float>(
        "scaleY", &m_scaleY, std::bind(&TransformedPositionElement::setScaleY, this, std::placeholders::_1));
    builder->registerProperty<float>(
        "scale", &m_scaleX, std::bind(&TransformedPositionElement::setScale, this, std::placeholders::_1));
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

} }
