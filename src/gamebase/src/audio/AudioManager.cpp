/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/audio/AudioManager.h>
#include "src/impl/global/GlobalTemporary.h"
#include "src/impl/global/GlobalResources.h"
#include "src/impl/global/Config.h"
#include <gamebase/tools/FileIO.h>
#include <boost/algorithm/string.hpp>

namespace gamebase {

void AudioManager::run(const std::string& path, int channel)
{
    impl::g_temp.audioManager.addAudio(path, channel);
}

void AudioManager::loop(const std::string& path, int channel)
{
    impl::g_temp.audioManager.loopAudio(path, channel);
}

void AudioManager::preload(const std::string& path)
{
    auto processedFilePath = boost::algorithm::replace_all_copy(path, "/", "\\");
    auto fullPath = impl::config().soundsPath + processedFilePath;
    if (fileInfo(fullPath).type == FileDesc::Directory)
        impl::globalResources().soundLibrary.preloadAll(processedFilePath);
    else
        impl::globalResources().soundLibrary.preload(processedFilePath);
}

void AudioManager::reset(int channel)
{
    impl::g_temp.audioManager.resetChannel(channel);
}

void AudioManager::reset()
{
    impl::g_temp.audioManager.reset();
}

bool AudioManager::isRunning(int channel) const
{
    return impl::g_temp.audioManager.isRunning(channel);
}

bool AudioManager::isEmpty(int channel) const
{
    return impl::g_temp.audioManager.isEmpty(channel);
}

void AudioManager::setSpeed(float speed, int channel)
{
    impl::g_temp.audioManager.setSpeed(speed, channel);
}

void AudioManager::setSpeed(float speed)
{
    impl::g_temp.audioManager.setSpeed(speed);
}

float AudioManager::speed(int channel) const
{
    return impl::g_temp.audioManager.speed(channel);
}

float AudioManager::speed() const
{
    return impl::g_temp.audioManager.speed();
}

void AudioManager::setVolume(float volume, int channel)
{
    impl::g_temp.audioManager.setVolume(volume, channel);
}

void AudioManager::setVolume(float volume)
{
    impl::g_temp.audioManager.setVolume(volume);
}

float AudioManager::volume(int channel) const
{
    return impl::g_temp.audioManager.volume(channel);
}

float AudioManager::volume() const
{
    return impl::g_temp.audioManager.volume();
}

void AudioManager::pause(int channel)
{
    impl::g_temp.audioManager.pause(channel);
}

void AudioManager::pause()
{
    impl::g_temp.audioManager.pause();
}

void AudioManager::resume(int channel)
{
    impl::g_temp.audioManager.resume(channel);
}

void AudioManager::resume()
{
    impl::g_temp.audioManager.resume();
}

bool AudioManager::isPaused(int channel) const
{
    return impl::g_temp.audioManager.isPaused(channel);
}

bool AudioManager::isPaused() const
{
    return impl::g_temp.audioManager.isPaused();
}

}
