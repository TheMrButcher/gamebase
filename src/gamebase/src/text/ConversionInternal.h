#pragma once

#include <gamebase/text/Conversion.h>
#include <vector>

namespace gamebase {
    
size_t utf8CharLen(char c);

std::vector<std::string> splitToUtf8Chars(const std::string& utf8Str);

}
