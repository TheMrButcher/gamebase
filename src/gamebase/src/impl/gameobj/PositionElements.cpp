/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameobj/PositionElement.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void PositionElement::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerVec2("offset", &m_pos.offset);
    builder->registerProperty("x", &m_pos.offset.x);
    builder->registerProperty("y", &m_pos.offset.y);
    builder->registerPropertyWithSetter<float>("angle", &m_angle,
        [this](float angle) { setAngle(angle); });
    builder->registerPropertyWithSetter<float>(
        "sx", &m_scaleX, [this](float sx) { setScaleX(sx); });
    builder->registerPropertyWithSetter<float>(
        "sy", &m_scaleY, [this](float sy) { setScaleY(sy); });
    builder->registerPropertyWithSetter<float>(
        "scale", &m_scaleX, [this](float scale) { setScale(scale); });
}

void PositionElement::serialize(Serializer& s) const
{
    s << "offset" << m_pos.offset << "angle" << m_angle << "scaleX" << m_scaleX << "scaleY" << m_scaleY;
}

std::unique_ptr<IObject> deserializeOffsettedPositionElement(Deserializer& deserializer)
{
    DESERIALIZE(Vec2, offset);
    return std::unique_ptr<PositionElement>(new PositionElement(1, 1, 0, offset));
}

std::unique_ptr<IObject> deserializeRotatedPositionElement(Deserializer& deserializer)
{
    DESERIALIZE(Vec2, offset);
    DESERIALIZE(float, angle);
    return std::unique_ptr<PositionElement>(new PositionElement(1, 1, angle, offset));
}

std::unique_ptr<IObject> deserializeTransformedPositionElement(Deserializer& deserializer)
{
    DESERIALIZE(Vec2, offset);
    DESERIALIZE(float, angle);
    DESERIALIZE(float, scaleX);
    DESERIALIZE(float, scaleY);
    return std::unique_ptr<PositionElement>(new PositionElement(scaleX, scaleY, angle, offset));
}

std::unique_ptr<IObject> deserializePositionElement(Deserializer& deserializer)
{
    return deserializeTransformedPositionElement(deserializer);
}

namespace {
class _GamebaseRegisterTransformedPositionElement {
public:
    _GamebaseRegisterTransformedPositionElement()
    {
        SerializableRegister::instance().registerType<PositionElement>("OffsettedPositionElement", deserializeOffsettedPositionElement);
        SerializableRegister::instance().registerType<PositionElement>("RotatedPositionElement", deserializeRotatedPositionElement);
        SerializableRegister::instance().registerType<PositionElement>("TransformedPositionElement", deserializeTransformedPositionElement);
    }
} _gamebaseRegisterTransformedPositionElement;
}

REGISTER_CLASS(PositionElement);

} }
