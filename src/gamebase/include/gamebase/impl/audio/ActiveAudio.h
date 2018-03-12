/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/audio/IAudio.h>
#include <unordered_set>
#include <vector>

namespace gamebase { namespace impl {

class ActiveAudio {
public:
    void step();
    void pushAudio(IAudio* audio);
    void removeAudio(IAudio* audio);

private:
    std::unordered_set<IAudio*> m_audioSet;
    std::vector<IAudio*> m_curAudioList;
};

} }
