#pragma once

#include <gamebase/GameBaseAPI.h>
#include <string>
#include <unordered_map>

namespace gamebase {

GAMEBASE_API void configurateFromFile(
    const std::string& fname, bool printStats = true);

GAMEBASE_API void configurateFromString(
    const std::string& configStr, bool printStats = true);

typedef std::unordered_map<std::string, std::string> Dictionary;

GAMEBASE_API const Dictionary& configAsDictionary();

}
