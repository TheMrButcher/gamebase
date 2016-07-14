/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <string>
#include <unordered_map>

namespace gamebase { namespace impl {

GAMEBASE_API void configurateFromFile(
    const std::string& fname, bool printStats = true);

GAMEBASE_API void configurateFromString(
    const std::string& configStr, bool printStats = true);

typedef std::unordered_map<std::string, std::string> Dictionary;

GAMEBASE_API const std::string& configAsString();

GAMEBASE_API const Dictionary& configAsDictionary();

inline const std::string& getValueFromConfig(
    const std::string& key, const std::string& defaultValue = "")
{
    const auto& config = configAsDictionary();
    auto it = config.find(key);
    if (it == config.end())
        return defaultValue;
    return it->second;
}

} }
