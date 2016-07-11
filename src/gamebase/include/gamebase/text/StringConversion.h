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
