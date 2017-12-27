/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/sound/ISound.h>
#include <unordered_set>
#include <vector>

namespace gamebase { namespace impl {

class ActiveSounds {
public:
    void step();
    void pushSound(ISound* sound);
    void removeSound(ISound* sound);

private:
    std::unordered_set<ISound*> m_sounds;
    std::vector<ISound*> m_curSounds;
};

} }
