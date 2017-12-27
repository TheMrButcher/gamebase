/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/sound/ActiveSounds.h>

namespace gamebase { namespace impl {

void ActiveSounds::step()
{
    m_curSounds.insert(m_curSounds.end(), m_sounds.begin(), m_sounds.end());
    for (auto sound : m_curSounds) {
        if (sound->isStopped())
            sound->kill();
    }
    m_curSounds.clear();
}

void ActiveSounds::pushSound(ISound* sound)
{
    m_sounds.insert(sound);
}

void ActiveSounds::removeSound(ISound* sound)
{
    m_sounds.erase(sound);
}

} }
