/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/audio/AudioManager.h>
#include "src/impl/global/GlobalResources.h"
#include "src/impl/global/Config.h"
#include <gamebase/impl/audio/Sound.h>
#include <gamebase/impl/audio/Music.h>
#include <gamebase/tools/FileIO.h>
#include <boost/algorithm/string.hpp>

namespace gamebase { namespace impl {

namespace {
std::shared_ptr<IAudio> tryLoadSound(const std::string& filePath)
{
    if (globalResources().soundLibrary.has(filePath))
        return std::make_shared<Sound>(filePath);
    auto fullPath = config().soundsPath + filePath;
    if (fileExists(fullPath))
        return std::make_shared<Sound>(filePath);
    return nullptr;
}

std::shared_ptr<IAudio> tryLoadMusic(const std::string& filePath)
{
    auto fullPath = config().musicPath + filePath;
    if (fileExists(fullPath))
        return std::make_shared<Music>(filePath);
    return nullptr;
}
}

AudioManager::AudioManager()
    : m_speed(1.f)
    , m_volume(1.f)
    , m_isPaused(false)
{
    AudioChannel defaultChannel(m_speed, m_volume, m_isPaused);
    defaultChannel.setParallel(true);
    m_channels.insert(std::make_pair(0, defaultChannel));
}

AudioManager::~AudioManager()
{
    reset();
}

std::shared_ptr<IAudio> AudioManager::addAudio(const std::string& filePath, int channelID)
{
    auto processedFilePath = boost::algorithm::replace_all_copy(filePath, "/", "\\");
    auto itType = m_pathToType.find(processedFilePath);
    std::shared_ptr<IAudio> audio;
    if (itType != m_pathToType.end()) {
        if (itType->second == Type::Sound) {
            audio = std::make_shared<Sound>(processedFilePath);
        } else {
            audio = std::make_shared<Music>(processedFilePath);
        }
    } else {
        if (audio = tryLoadSound(processedFilePath)) {
            m_pathToType[processedFilePath] = Type::Sound;
        } else {
            if (audio = tryLoadMusic(processedFilePath)) {
                m_pathToType[processedFilePath] = Type::Music;
            } else {
                THROW_EX() << "Can't find sound file: " << processedFilePath;
            }
        }
    }

    auto itChannel = m_channels.find(channelID);
    if (itChannel == m_channels.end())
        itChannel = m_channels.insert(std::make_pair(
            channelID, AudioChannel(m_speed, m_volume, m_isPaused))).first;
    itChannel->second.add(audio);
    return audio;
}

std::shared_ptr<IAudio> AudioManager::loopAudio(const std::string& filePath, int channelID)
{
    auto audio = addAudio(filePath, channelID);
    audio->setLoop(true);
    return audio;
}

void AudioManager::step()
{
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it)
        it->second.step();
}

void AudioManager::resetChannel(int channelID)
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return;

    it->second.reset();
}

void AudioManager::reset()
{
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it)
        it->second.reset();
}

void AudioManager::setSpeed(float speed, int channelID)
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end()) {
        m_channels.insert(std::make_pair(
            channelID, AudioChannel(speed, m_volume, m_isPaused)));
        return;
    }

    it->second.setSpeed(speed);
}

void AudioManager::setSpeed(float speed)
{
    m_speed = speed;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        auto& channel = it->second;
        channel.setSpeed(speed);
    }
}

float AudioManager::speed(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return m_speed;
    return it->second.speed();
}

void AudioManager::setVolume(float volume, int channelID)
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end()) {
        m_channels.insert(std::make_pair(
            channelID, AudioChannel(m_speed, volume, m_isPaused)));
        return;
    }

    it->second.setVolume(volume);
}

void AudioManager::setVolume(float volume)
{
    m_volume = volume;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        auto& channel = it->second;
        channel.setVolume(m_volume);
    }
}

float AudioManager::volume(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return m_volume;
    return it->second.volume();
}

void AudioManager::pause(int channelID)
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end()) {
        m_channels.insert(std::make_pair(
            channelID, AudioChannel(m_speed, m_volume, true)));
        return;
    }

    it->second.pause();
}

void AudioManager::pause()
{
    m_isPaused = true;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        auto& channel = it->second;
        channel.pause();
    }
}

void AudioManager::resume(int channelID)
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end()) {
        m_channels.insert(std::make_pair(
            channelID, AudioChannel(m_speed, m_volume, false)));
        return;
    }

    it->second.resume();
}

void AudioManager::resume()
{
    m_isPaused = false;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        auto& channel = it->second;
        channel.resume();
    }
}

bool AudioManager::isPaused(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return true;
    return it->second.isPaused();
}

bool AudioManager::isEmpty(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return true;
    return it->second.isEmpty();
}

bool AudioManager::isRunning(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return false;
    return it->second.isRunning();
}

} }
