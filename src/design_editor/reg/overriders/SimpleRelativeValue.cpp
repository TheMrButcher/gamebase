/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "SimpleRelativeValue.h"

namespace gamebase { namespace editor {

void SimpleRelativeValue::set(impl::RelType::Enum type, double value)
{
    switch (type) {
    case impl::RelType::Identic: set(SimpleRelType::Percents, 1.0); break;
    case impl::RelType::Ratio:   set(SimpleRelType::Percents, value); break;
    case impl::RelType::Pixels:  set(SimpleRelType::Pixels, value); break;
    default: THROW_EX() << "Unexpected RelType: " << static_cast<int>(type);
    }
}

void SimpleRelativeValue::set(SimpleRelType::Enum type, double value)
{
    m_type = type;
    m_value = round(value, m_type);
}

impl::RelativeValue SimpleRelativeValue::toRelativeValue() const
{
    double v = value();
    if (m_type == SimpleRelType::Pixels)
        return impl::RelativeValue(impl::RelType::Pixels, static_cast<float>(v));

    static const double EPSILON = 0.001;
    if (std::abs(v - 1.0) < EPSILON)
        return impl::RelativeValue();
    return impl::RelativeValue(impl::RelType::Ratio, static_cast<float>(v));
}

double SimpleRelativeValue::value() const
{
    return m_value;
}

void SimpleRelativeValue::serialize(impl::Serializer& s) const
{
	s << "type" << m_type << "value" << (m_type == SimpleRelType::Pixels ? 1.0 : 100.0) * value();
}

std::unique_ptr<impl::IObject> deserializeSimpleRelativeValue(impl::Deserializer& deserializer)
{
    DESERIALIZE(SimpleRelType::Enum, type);
    DESERIALIZE(double, value);
    return std::unique_ptr<impl::IObject>(new SimpleRelativeValue(
		type,
		type == SimpleRelType::Pixels ? value : value * 0.01));
}

REGISTER_CLASS(SimpleRelativeValue);

double round(double value, SimpleRelType::Enum type)
{
	int coef = type == SimpleRelType::Pixels ? 1000 : 100000;
	return static_cast<int>(value * coef + (value >= 0 ? 0.5 : -0.5)) / static_cast<double>(coef);
}

} }
