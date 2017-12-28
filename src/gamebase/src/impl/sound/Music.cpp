/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/sound/Music.h>
#include "src/impl/global/GlobalTemporary.h"
#include "src/impl/global/Config.h"
#include <gamebase/tools/Exception.h>
#include <SFML/Audio/Music.hpp>

namespace gamebase { namespace impl {

namespace {
std::string makePath(const std::string& path)
{
    return config().musicPath + path;
}
}

Music::Music(const std::string& filePath)
    : m_filePath(filePath)
    , m_time(0)
    , m_speed(1.f)
    , m_volume(1.f)
    , m_loop(false)
{}

Music::~Music()
{
    stop();
}

void Music::start()
{
    ensureMusicCreated();
    m_music->stop();
    m_music->play();
}

void Music::stop()
{
    if (!m_music)
        return;
    m_music->stop();
    kill();
}

void Music::resume()
{
    if (isRunning())
        return;
    ensureMusicCreated();
    m_music->play();
    if (m_time != 0)
        m_music->setPlayingOffset(sf::milliseconds(sf::Int32(m_time)));
}

void Music::pause()
{
    if (!isRunning())
        return;
    m_music->pause();
}

bool Music::isRunning() const
{
    return m_music && m_music->getStatus() == sf::Music::Playing;
}

bool Music::isPaused() const
{
    return m_music && m_music->getStatus() == sf::Music::Paused;
}

bool Music::isStopped() const
{
    return !m_music || m_music->getStatus() == sf::Music::Stopped;
}

void Music::setTime(Time time)
{
    m_time = time;
    if (m_music)
        m_music->setPlayingOffset(sf::milliseconds(sf::Int32(m_time)));
}

Time Music::time() const
{
    if (m_music)
        return Time(m_music->getPlayingOffset().asMilliseconds());
    return m_time;
}

void Music::setSpeed(float speed)
{
    m_speed = speed;
    if (m_music)
        m_music->setPitch(speed);
}

float Music::speed() const
{
    return m_speed;
}

void Music::setVolume(float volume)
{
    m_volume = volume;
    if (m_music)
        m_music->setVolume(volume * 100);
}

float Music::volume() const
{
    return m_volume;
}

void Music::setLoop(bool value)
{
    m_loop = value;
    if (m_music)
        m_music->setLoop(value);
}

bool Music::isLoop() const
{
    return m_loop;
}

void Music::kill()
{
    m_music.reset();
    g_temp.activeSounds.removeSound(this);
    m_time = 0;
}

void Music::ensureMusicCreated()
{
    if (m_music)
        return;
    m_music = std::make_shared<sf::Music>();
    if (!m_music->openFromFile(makePath(m_filePath)))
        THROW_EX() << "Error while loading music: " << m_filePath;
    m_music->setAttenuation(0.f);
    m_music->setPitch(m_speed);
    m_music->setVolume(m_volume * 100.f);
    m_music->setLoop(m_loop);
    g_temp.activeSounds.pushSound(this);
}

} }
