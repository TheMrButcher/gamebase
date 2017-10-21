/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/text/Conversion.h>

namespace gamebase {

inline std::string toLocal(const std::string& unicodeString)
{
    return impl::convertToLocal(unicodeString);
}

inline std::string toUnicode(const std::string& localString)
{
    return impl::convertToUtf8(localString);
}

}
