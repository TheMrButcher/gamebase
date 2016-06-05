#pragma once

#include <gamebase/GameBaseAPI.h>
#include <string>

namespace gamebase { namespace impl {

GAMEBASE_API std::string convertToUtf8(const std::string& localStr);

GAMEBASE_API std::string convertToLocal(const std::string& utf8Str);

} }
