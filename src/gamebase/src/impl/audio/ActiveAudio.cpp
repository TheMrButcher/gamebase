/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/audio/ActiveAudio.h>

namespace gamebase { namespace impl {

void ActiveAudio::step()
{
    m_curAudioList.insert(m_curAudioList.end(), m_audioSet.begin(), m_audioSet.end());
    for (auto audio : m_curAudioList) {
        if (audio->isStopped())
            audio->kill();
    }
    m_curAudioList.clear();
}

void ActiveAudio::pushAudio(IAudio* audio)
{
    m_audioSet.insert(audio);
}

void ActiveAudio::removeAudio(IAudio* audio)
{
    m_audioSet.erase(audio);
}

} }
