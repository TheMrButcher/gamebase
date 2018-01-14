/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/audio/AudioChannel.h>
#include <algorithm>

namespace gamebase { namespace impl {

AudioChannel::AudioChannel(float speed, float volume, bool isPaused)
    : m_isParallel(false)
    , m_speed(speed)
    , m_volume(volume)
    , m_isPaused(isPaused)
{}

void AudioChannel::add(const std::shared_ptr<IAudio>& audio)
{
    if (m_speed > 0.f)
        audio->setSpeed(m_speed);
    audio->setVolume(m_volume);
    if ((isEmpty() || m_isParallel) && m_speed > 0.f && !m_isPaused)
        audio->start();
    m_audioQueue.push_back(audio);
}

void AudioChannel::step()
{
    if (!isRunning())
        return;
    if (m_isParallel) {
        m_audioQueue.erase(
            std::remove_if(m_audioQueue.begin(), m_audioQueue.end(),
                [](const auto& audio) { return !audio->isRunning(); }),
            m_audioQueue.end());
        return;
    }
    if (!m_audioQueue.front()->isRunning()) {
        m_audioQueue.pop_front();
        if (!m_audioQueue.empty())
            m_audioQueue.front()->start();
    }
}

void AudioChannel::reset()
{
    for (const auto& audio : m_audioQueue)
        audio->stop();
    m_audioQueue.clear();
}

void AudioChannel::setParallel(bool value)
{
    m_isParallel = true;
    if (m_speed > 0.f && !m_isPaused) {
        for (const auto& audio : m_audioQueue) {
            if (!audio->isRunning())
                audio->start();
        }
    }
}

void AudioChannel::setSpeed(float speed)
{
    m_speed = speed;
    if (m_speed > 0.f) {
        for (const auto& audio : m_audioQueue)
            audio->setSpeed(speed);
    } else {
        if (m_isParallel) {
            for (const auto& audio : m_audioQueue) {
                if (audio->isRunning())
                    audio->pause();
            }
        } else {
            if (!isEmpty()) {
                auto audio = m_audioQueue.front();
                if (audio->isRunning())
                    audio->pause();
            }
        }
    }
}

void AudioChannel::setVolume(float volume)
{
    m_volume = volume;
    for (const auto& audio : m_audioQueue)
        audio->setVolume(volume);
}

void AudioChannel::pause()
{
    m_isPaused = true;
    for (const auto& audio : m_audioQueue) {
        if (audio->isRunning())
            audio->pause();
    }
}

void AudioChannel::resume()
{
    m_isPaused = false;
    if (m_speed <= 0.f)
        return;
    if (m_isParallel) {
        for (const auto& audio : m_audioQueue) {
            if (!audio->isRunning())
                audio->resume();
        }
    } else {
        if (!isEmpty()) {
            auto audio = m_audioQueue.front();
            if (!audio->isRunning())
                audio->resume();
        }
    }
}

bool AudioChannel::isEmpty() const
{
    return m_audioQueue.empty();
}

bool AudioChannel::isRunning() const
{
    return !isEmpty() && m_speed > 0.f && !isPaused();
}

} }
