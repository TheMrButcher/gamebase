/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/sound/SoundManager.h>
#include "src/impl/global/GlobalTemporary.h"
#include "src/impl/global/GlobalResources.h"
#include "src/impl/global/Config.h"
#include <gamebase/tools/FileIO.h>
#include <boost/algorithm/string.hpp>

namespace gamebase {

void SoundManager::run(const std::string& path, int channel)
{
    impl::g_temp.soundManager.addSound(path, channel);
}

void SoundManager::loop(const std::string& path, int channel)
{
    impl::g_temp.soundManager.loopSound(path, channel);
}

void SoundManager::preload(const std::string& path)
{
    auto processedFilePath = boost::algorithm::replace_all_copy(path, "/", "\\");
    auto fullPath = impl::config().soundsPath + processedFilePath;
    if (fileInfo(fullPath).type == FileDesc::Directory)
        impl::globalResources().soundLibrary.preloadAll(processedFilePath);
    else
        impl::globalResources().soundLibrary.preload(processedFilePath);
}

void SoundManager::reset(int channel)
{
    impl::g_temp.soundManager.resetChannel(channel);
}

void SoundManager::reset()
{
    impl::g_temp.soundManager.reset();
}

bool SoundManager::isRunning(int channel) const
{
    return impl::g_temp.soundManager.isRunning(channel);
}

bool SoundManager::isEmpty(int channel) const
{
    return impl::g_temp.soundManager.isEmpty(channel);
}

void SoundManager::setSpeed(float speed, int channel)
{
    impl::g_temp.soundManager.setSpeed(speed, channel);
}

void SoundManager::setSpeed(float speed)
{
    impl::g_temp.soundManager.setSpeed(speed);
}

float SoundManager::speed(int channel) const
{
    return impl::g_temp.soundManager.speed(channel);
}

float SoundManager::speed() const
{
    return impl::g_temp.soundManager.speed();
}

void SoundManager::setVolume(float volume, int channel)
{
    impl::g_temp.soundManager.setVolume(volume, channel);
}

void SoundManager::setVolume(float volume)
{
    impl::g_temp.soundManager.setVolume(volume);
}

float SoundManager::volume(int channel) const
{
    return impl::g_temp.soundManager.volume(channel);
}

float SoundManager::volume() const
{
    return impl::g_temp.soundManager.volume();
}

void SoundManager::pause(int channel)
{
    impl::g_temp.soundManager.pause(channel);
}

void SoundManager::pause()
{
    impl::g_temp.soundManager.pause();
}

void SoundManager::resume(int channel)
{
    impl::g_temp.soundManager.resume(channel);
}

void SoundManager::resume()
{
    impl::g_temp.soundManager.resume();
}

bool SoundManager::isPaused(int channel) const
{
    return impl::g_temp.soundManager.isPaused(channel);
}

bool SoundManager::isPaused() const
{
    return impl::g_temp.soundManager.isPaused();
}

}
