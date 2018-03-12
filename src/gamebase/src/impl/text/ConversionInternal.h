/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/text/Conversion.h>
#include <vector>

namespace gamebase { namespace impl {
    
size_t utf8CharLen(
    std::string::const_iterator it, std::string::const_iterator itEnd);

std::vector<std::string> splitToUtf8Chars(const std::string& utf8Str);

std::string localToUtf8Code(char c);

void initConversionMaps();

} }
