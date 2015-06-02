#pragma once

#include <gamebase/text/Conversion.h>
#include <vector>

namespace gamebase {
    
size_t utf8CharLen(
    std::string::const_iterator it, std::string::const_iterator itEnd);

std::vector<std::string> splitToUtf8Chars(const std::string& utf8Str);

std::string localToUtf8Code(char c);

void initConversionMaps();

}
