/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/sound/SoundManager.h>

namespace gamebase { namespace impl {

SoundChannel::SoundChannel(float speed, float volume, bool isPaused)
    : m_isParallel(false)
    , m_speed(speed)
    , m_volume(volume)
    , m_isPaused(isPaused)
{}

void SoundChannel::addSound(const std::shared_ptr<ISound>& sound)
{
    if (m_speed > 0.f)
        sound->setSpeed(m_speed);
    sound->setVolume(m_volume);
    if ((isEmpty() || m_isParallel) && m_speed > 0.f && !m_isPaused)
        sound->start();
    m_sounds.push_back(sound);
}

void SoundChannel::step()
{
    if (!isRunning())
        return;
    if (m_isParallel) {
        m_sounds.erase(
            std::remove_if(m_sounds.begin(), m_sounds.end(),
                [](const auto& sound) { return !sound->isRunning(); }),
            m_sounds.end());
        return;
    }
    if (!m_sounds.front()->isRunning()) {
        m_sounds.pop_front();
        if (!m_sounds.empty())
            m_sounds.front()->start();
    }
}

void SoundChannel::reset()
{
    for (const auto& sound : m_sounds)
        sound->stop();
    m_sounds.clear();
}

void SoundChannel::setParallel(bool value)
{
    m_isParallel = true;
    if (m_speed > 0.f && !m_isPaused) {
        for (const auto& sound : m_sounds) {
            if (!sound->isRunning())
                sound->start();
        }
    }
}

void SoundChannel::setSpeed(float speed)
{
    m_speed = speed;
    if (m_speed > 0.f) {
        for (const auto& sound : m_sounds)
            sound->setSpeed(speed);
    } else {
        if (m_isParallel) {
            for (const auto& sound : m_sounds) {
                if (sound->isRunning())
                    sound->pause();
            }
        } else {
            if (!isEmpty()) {
                auto sound = m_sounds.front();
                if (sound->isRunning())
                    sound->pause();
            }
        }
    }
}

void SoundChannel::setVolume(float volume)
{
    m_volume = volume;
    for (const auto& sound : m_sounds)
        sound->setVolume(volume);
}

void SoundChannel::pause()
{
    m_isPaused = true;
    for (const auto& sound : m_sounds) {
        if (sound->isRunning())
            sound->pause();
    }
}

void SoundChannel::resume()
{
    m_isPaused = false;
    if (m_speed <= 0.f)
        return;
    if (m_isParallel) {
        for (const auto& sound : m_sounds) {
            if (!sound->isRunning())
                sound->resume();
        }
    } else {
        if (!isEmpty()) {
            auto sound = m_sounds.front();
            if (!sound->isRunning())
                sound->resume();
        }
    }
}

bool SoundChannel::isEmpty() const
{
    return m_sounds.empty();
}

bool SoundChannel::isRunning() const
{
    return !isEmpty() && m_speed > 0.f && !isPaused();
}

} }
