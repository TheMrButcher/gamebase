/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "SimpleRelativeValue.h"

namespace gamebase { namespace editor {

namespace {
float valueToFloat(int value)
{
	return value * 0.0009765625f;
}

int valueToInt(float value)
{
	return static_cast<int>(value * 1024 + (value >= 0 ? 0.5f : -0.5f));
}

float round(float value)
{
	return valueToFloat(valueToInt(value));
}
}
    
void SimpleRelativeValue::set(impl::RelType::Enum type, float value)
{
    switch (type) {
    case impl::RelType::Identic: set(SimpleRelType::Percents, 100.0f); break;
    case impl::RelType::Ratio:   set(SimpleRelType::Percents, 100.0f * value); break;
    case impl::RelType::Pixels:  set(SimpleRelType::Pixels, value); break;
    default: THROW_EX() << "Unexpected RelType: " << static_cast<int>(type);
    }
}

void SimpleRelativeValue::set(SimpleRelType::Enum type, float value)
{
    m_type = type;
    m_value = valueToInt(value);
}

impl::RelativeValue SimpleRelativeValue::toRelativeValue() const
{
    float v = value();
    if (m_type == SimpleRelType::Pixels)
        return impl::RelativeValue(impl::RelType::Pixels, v);

    static const float EPSILON = 0.01f;
    if (std::abs(v - 100.0f) < EPSILON)
        return impl::RelativeValue();
    return impl::RelativeValue(impl::RelType::Ratio, v / 100.0f);
}

float SimpleRelativeValue::value() const
{
    return valueToFloat(m_value);
}

void SimpleRelativeValue::serialize(impl::Serializer& s) const
{
    s << "type" << m_type << "value" << value();
}

std::unique_ptr<impl::IObject> deserializeSimpleRelativeValue(impl::Deserializer& deserializer)
{
    DESERIALIZE(SimpleRelType::Enum, type);
    DESERIALIZE(float, value);
    return std::unique_ptr<impl::IObject>(new SimpleRelativeValue(type, value));
}

REGISTER_CLASS(SimpleRelativeValue);

} }
