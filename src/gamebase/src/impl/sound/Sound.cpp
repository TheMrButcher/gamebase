/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/sound/Sound.h>
#include "src/impl/global/GlobalResources.h"
#include "src/impl/global/GlobalTemporary.h"
#include <gamebase/impl/sound/SoundLibrary.h>
#include <gamebase/math/Math.h>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace gamebase { namespace impl {

Sound::Sound(const std::string& filePath)
    : m_time(0)
    , m_speed(1.f)
    , m_volume(1.f)
    , m_loop(false)
{
    m_buffer = globalResources().soundLibrary.load(filePath);
}

Sound::~Sound()
{
    stop();
}

void Sound::start()
{
    ensureSoundCreated();
    m_sound->stop();
    m_sound->play();
}

void Sound::stop()
{
    if (!m_sound)
        return;
    m_sound->stop();
    kill();
}

void Sound::resume()
{
    if (isRunning())
        return;
    ensureSoundCreated();
    m_sound->play();
    if (m_time != 0)
        m_sound->setPlayingOffset(sf::milliseconds(sf::Int32(m_time)));
}

void Sound::pause()
{
    if (!isRunning())
        return;
    m_sound->pause();
}

bool Sound::isRunning() const
{
    return m_sound && m_sound->getStatus() == sf::Sound::Playing;
}

bool Sound::isPaused() const
{
    return m_sound && m_sound->getStatus() == sf::Sound::Paused;
}

bool Sound::isStopped() const
{
    return !m_sound || m_sound->getStatus() == sf::Sound::Stopped;
}

void Sound::setTime(Time time)
{
    m_time = time;
    if (m_sound)
        m_sound->setPlayingOffset(sf::milliseconds(sf::Int32(m_time)));
}

Time Sound::time() const
{
    if (m_sound)
        return Time(m_sound->getPlayingOffset().asMilliseconds());
    return m_time;
}

void Sound::setSpeed(float speed)
{
    m_speed = std::max(speed, 0.f);
    if (m_sound)
        m_sound->setPitch(speed);
}

float Sound::speed() const
{
    return m_speed;
}

void Sound::setVolume(float volume)
{
    m_volume = clamp(volume, 0.f, 1.f);
    if (m_sound)
        m_sound->setVolume(volume * 100);
}

float Sound::volume() const
{
    return m_volume;
}

void Sound::setLoop(bool value)
{
    m_loop = value;
    if (m_sound)
        m_sound->setLoop(value);
}

bool Sound::isLoop() const
{
    return m_loop;
}

void Sound::kill()
{
    m_sound.reset();
    g_temp.activeSounds.removeSound(this);
    m_time = 0;
}

void Sound::ensureSoundCreated()
{
    if (m_sound)
        return;
    m_sound = std::make_shared<sf::Sound>(*m_buffer);
    m_sound->setAttenuation(0.f);
    m_sound->setPitch(m_speed);
    m_sound->setVolume(m_volume * 100.f);
    m_sound->setLoop(m_loop);
    g_temp.activeSounds.pushSound(this);
}

} }
