/**
* Copyright (c) 2017 Slavnejshev Filipp
* This file is licensed under the terms of the MIT license.
*/

#pragma once

#include <gamebase/impl/sound/Sound.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Sound {
public:
    void insert(const std::string& key, const std::string& value);
    std::string operator[](const std::string& key) const;
    std::string get(const std::string& key) const;
    std::string get(const std::string& key, int index) const;

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL(Sound, Sound);
};

inline Sound loadSound(const std::string& filePath);

/////////////// IMPLEMENTATION ///////////////////

inline Sound loadSound(const std::string& filePath) {}


}
