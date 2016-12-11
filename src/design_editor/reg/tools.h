/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <gamebase/impl/engine/RelativeValue.h>

#pragma once

namespace gamebase { namespace editor {

inline impl::RelativeValue makeValue(impl::RelType::Enum type, float value)
{
    return impl::RelativeValue(type, value);
}

inline impl::RelativeValue identic()
{
    return makeValue(impl::RelType::Identic, 0);
}

inline impl::RelativeValue pixels(float value)
{
    return makeValue(impl::RelType::Pixels, value);
}

inline impl::RelativeValue zeroValue()
{
    return pixels(0);
}

inline bool isZero(const impl::RelativeValue& value)
{
    return value.type() == impl::RelType::Pixels && value.value() == 0;
}

inline bool isIdentic(const impl::RelativeValue& value)
{
    return value.type() == impl::RelType::Identic;
}

} }
