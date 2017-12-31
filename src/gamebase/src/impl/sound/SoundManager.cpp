/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/sound/SoundManager.h>
#include "src/impl/global/GlobalResources.h"
#include "src/impl/global/Config.h"
#include <gamebase/impl/sound/Sound.h>
#include <gamebase/impl/sound/Music.h>
#include <gamebase/tools/FileIO.h>
#include <boost/algorithm/string.hpp>

namespace gamebase { namespace impl {

namespace {
std::shared_ptr<ISound> tryLoadSound(const std::string& filePath)
{
    if (globalResources().soundLibrary.has(filePath))
        return std::make_shared<Sound>(filePath);
    auto fullPath = config().soundsPath + filePath;
    if (fileExists(fullPath))
        return std::make_shared<Sound>(filePath);
    return nullptr;
}

std::shared_ptr<ISound> tryLoadMusic(const std::string& filePath)
{
    auto fullPath = config().musicPath + filePath;
    if (fileExists(fullPath))
        return std::make_shared<Music>(filePath);
    return nullptr;
}
}

SoundManager::SoundManager()
    : m_speed(1.f)
    , m_volume(1.f)
    , m_isPaused(false)
{
    SoundChannel defaultChannel(m_speed, m_volume, m_isPaused);
    defaultChannel.setParallel(true);
    m_channels.insert(std::make_pair(0, defaultChannel));
}

SoundManager::~SoundManager()
{
    reset();
}

std::shared_ptr<ISound> SoundManager::addSound(const std::string& filePath, int channelID)
{
    auto processedFilePath = boost::algorithm::replace_all_copy(filePath, "/", "\\");
    auto itType = m_pathToType.find(processedFilePath);
    std::shared_ptr<ISound> sound;
    if (itType != m_pathToType.end()) {
        if (itType->second == Type::Sound) {
            sound = std::make_shared<Sound>(processedFilePath);
        } else {
            sound = std::make_shared<Music>(processedFilePath);
        }
    } else {
        if (sound = tryLoadSound(processedFilePath)) {
            m_pathToType[processedFilePath] = Type::Sound;
        } else {
            if (sound = tryLoadMusic(processedFilePath)) {
                m_pathToType[processedFilePath] = Type::Music;
            } else {
                THROW_EX() << "Can't find sound file: " << processedFilePath;
            }
        }
    }

    auto itChannel = m_channels.find(channelID);
    if (itChannel == m_channels.end())
        itChannel = m_channels.insert(std::make_pair(
            channelID, SoundChannel(m_speed, m_volume, m_isPaused))).first;
    itChannel->second.addSound(sound);
    return sound;
}

std::shared_ptr<ISound> SoundManager::loopSound(const std::string& filePath, int channelID)
{
    auto sound = addSound(filePath, channelID);
    sound->setLoop(true);
    return sound;
}

void SoundManager::step()
{
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it)
        it->second.step();
}

void SoundManager::resetChannel(int channelID)
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return;

    it->second.reset();
}

void SoundManager::reset()
{
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it)
        it->second.reset();
}

void SoundManager::setSpeed(float speed, int channelID)
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end()) {
        m_channels.insert(std::make_pair(
            channelID, SoundChannel(speed, m_volume, m_isPaused)));
        return;
    }

    it->second.setSpeed(speed);
}

void SoundManager::setSpeed(float speed)
{
    m_speed = speed;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        auto& channel = it->second;
        channel.setSpeed(speed);
    }
}

float SoundManager::speed(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return m_speed;
    return it->second.speed();
}

void SoundManager::setVolume(float volume, int channelID)
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end()) {
        m_channels.insert(std::make_pair(
            channelID, SoundChannel(m_speed, volume, m_isPaused)));
        return;
    }

    it->second.setVolume(volume);
}

void SoundManager::setVolume(float volume)
{
    m_volume = volume;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        auto& channel = it->second;
        channel.setVolume(m_volume);
    }
}

float SoundManager::volume(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return m_volume;
    return it->second.volume();
}

void SoundManager::pause(int channelID)
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end()) {
        m_channels.insert(std::make_pair(
            channelID, SoundChannel(m_speed, m_volume, true)));
        return;
    }

    it->second.pause();
}

void SoundManager::pause()
{
    m_isPaused = true;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        auto& channel = it->second;
        channel.pause();
    }
}

void SoundManager::resume(int channelID)
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end()) {
        m_channels.insert(std::make_pair(
            channelID, SoundChannel(m_speed, m_volume, false)));
        return;
    }

    it->second.resume();
}

void SoundManager::resume()
{
    m_isPaused = false;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        auto& channel = it->second;
        channel.resume();
    }
}

bool SoundManager::isPaused(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return true;
    return it->second.isPaused();
}

bool SoundManager::isEmpty(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return true;
    return it->second.isEmpty();
}

bool SoundManager::isRunning(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return false;
    return it->second.isRunning();
}

} }
