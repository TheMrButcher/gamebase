#pragma once

#include <gamebase/GameBaseAPI.h>
#include <string>

namespace gamebase {

GAMEBASE_API void configurateFromFile(
    const std::string& fname, bool printStats = true);

GAMEBASE_API void configurateFromString(
    const std::string& configStr, bool printStats = true);

}
