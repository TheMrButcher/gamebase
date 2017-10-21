/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <reg/overriders/RelativeValueInUI.h>

#pragma once

namespace gamebase { namespace editor {

inline impl::RelativeValue makeValue(impl::RelType::Enum type, float value)
{
    return impl::RelativeValue(type, value);
}

inline impl::RelativeValue identic()
{
    return makeValue(impl::RelType::Ratio, 1.0f);
}

inline impl::RelativeValue pixels(float value)
{
    return makeValue(impl::RelType::Pixels, value);
}

inline impl::RelativeValue zeroValue()
{
    return pixels(0);
}

inline bool isZero(const RelativeValueInUI& value)
{
    return value.type() == impl::RelType::Pixels && value.value() == 0;
}

inline bool isIdentic(const RelativeValueInUI& value)
{
	static const double EPSILON = 0.001;
    return value.type() == impl::RelType::Ratio && std::abs(value.value() - 1.0) < EPSILON;
}

} }
