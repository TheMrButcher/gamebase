/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "RelativeValueInUI.h"
#include "SimpleRelativeValue.h"

namespace gamebase { namespace editor {

namespace {
SimpleRelType::Enum toSimple(impl::RelType::Enum type)
{
	return type == impl::RelType::Ratio ? SimpleRelType::Percents : SimpleRelType::Pixels;
}
}

void RelativeValueInUI::set(impl::RelType::Enum type, double value)
{
	if (type == impl::RelType::Identic) {
		set(impl::RelType::Ratio, 1.0);
		return;
	}
    m_type = type;
	m_value = round(value, toSimple(m_type));
}

impl::RelativeValue RelativeValueInUI::toRelativeValue() const
{
    double v = value();
    static const double EPSILON = 0.001;
	if (m_type == impl::RelType::Ratio && std::abs(v - 1.0) < EPSILON)
        return impl::RelativeValue();
    return impl::RelativeValue(m_type, static_cast<float>(v));
}

double RelativeValueInUI::value() const
{
    return m_value;
}

void RelativeValueInUI::serialize(impl::Serializer& s) const
{
	s << "type" << m_type << "value" << (m_type == impl::RelType::Ratio ? 100.0 : 1.0) * value();
}

std::unique_ptr<impl::IObject> deserializeRelativeValueInUI(impl::Deserializer& deserializer)
{
	DESERIALIZE(impl::RelType::Enum, type);
    DESERIALIZE(double, value);
    return std::unique_ptr<impl::IObject>(new RelativeValueInUI(
		type,
		type == impl::RelType::Ratio ? value * 0.01 : value));
}

REGISTER_CLASS(RelativeValueInUI);

} }
