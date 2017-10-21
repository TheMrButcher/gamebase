/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/anim/AnimationManager.h>
#include "src/impl/global/GlobalTemporary.h"

namespace gamebase { namespace impl {

AnimationManager::~AnimationManager()
{
    reset();
}

void AnimationManager::start()
{
    m_isStarted = true;
    countRunningChannels();
    if (m_runningChannelsNum)
        g_temp.currentAnimations.insert(this);
}

void AnimationManager::addAnimation(const std::shared_ptr<IAnimation>& animation, int channelID)
{
    if (!animation)
        return;
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        it = m_channels.insert(std::make_pair(channelID, Channel(m_speed, m_isPaused))).first;

    bool wasChannelRunning = isRunning(it->second);
    it->second.animations.push_back(animation);
    channelWasChanged(wasChannelRunning, isRunning(it->second));
}

void AnimationManager::step() const
{
    auto time = TimeState::time(m_type).delta;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        auto& channel = it->second;

        if (!isRunning(channel))
            continue;

        bool needStart = !channel.isStarted;
        channel.isStarted = true;

        float channelTime = channel.speed * time + channel.overTime;
        Time curTime = static_cast<Time>(channelTime);
        channel.overTime = channelTime - curTime;

        while (!channel.animations.empty()) {
            auto& animation = channel.animations.front();
            if (needStart) {
                animation->start();
                needStart = false;
            }
            curTime = animation->step(curTime);
            if (!animation->isFinished())
                break;
            channel.animations.pop_front();
            needStart = true;
        }
        if (channel.animations.empty()) {
            channel.reset();
            m_runningChannelsNum--;
        }
    }
    if (!m_runningChannelsNum)
        g_temp.currentAnimations.erase(this);
}

void AnimationManager::resetChannel(int channelID)
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return;

    auto& channel = it->second;
    bool wasChannelRunning = isRunning(channel);
    channel.reset();
    channelWasChanged(wasChannelRunning, false);
}

void AnimationManager::reset()
{
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it)
        it->second.reset();

    if (m_runningChannelsNum) {
        m_runningChannelsNum = 0;
        g_temp.currentAnimations.erase(this);
    }
}

void AnimationManager::setSpeed(int channelID, float speed)
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end()) {
        it = m_channels.insert(std::make_pair(channelID, Channel(speed, m_isPaused))).first;
        return;
    }

    bool wasChannelRunning = isRunning(it->second);
    it->second.speed = speed;
    channelWasChanged(wasChannelRunning, isRunning(it->second));
}

void AnimationManager::setSpeed(float speed)
{
    m_speed = speed;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        auto& channel = it->second;
        channel.speed = speed;
    }
    countRunningChannels();
    if (m_runningChannelsNum > 0)
        g_temp.currentAnimations.insert(this);
    else
        g_temp.currentAnimations.erase(this);
}

float AnimationManager::speed(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return 0;
    return it->second.speed;
}

void AnimationManager::pause(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end()) {
        it = m_channels.insert(std::make_pair(channelID, Channel(m_speed, true))).first;
        return;
    }

    auto& channel = it->second;
    bool wasChannelRunning = isRunning(channel);
    channel.isPaused = true;
    channelWasChanged(wasChannelRunning, false);
}

void AnimationManager::pause()
{
    m_isPaused = true;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        auto& channel = it->second;
        channel.isPaused = true;
    }

    if (m_runningChannelsNum) {
        m_runningChannelsNum = 0;
        g_temp.currentAnimations.erase(this);
    }
}

void AnimationManager::resume(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end()) {
        it = m_channels.insert(std::make_pair(channelID, Channel(m_speed, false))).first;
        return;
    }

    bool wasChannelRunning = isRunning(it->second);
    it->second.isPaused = false;
    channelWasChanged(wasChannelRunning, isRunning(it->second));
}

void AnimationManager::resume()
{
    m_isPaused = false;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        auto& channel = it->second;
        channel.isPaused = false;
    }
    countRunningChannels();
    if (m_runningChannelsNum > 0)
        g_temp.currentAnimations.insert(this);
}

bool AnimationManager::isPaused(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return true;
    return it->second.isPaused;
}

bool AnimationManager::isEmpty(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return true;
    return it->second.animations.empty();
}

bool AnimationManager::isRunning(int channelID) const
{
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        return false;
    return isRunning(it->second);
}

void AnimationManager::countRunningChannels()
{
    m_runningChannelsNum = 0;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it)
        if (isRunning(it->second))
            m_runningChannelsNum++;
}

void AnimationManager::channelWasChanged(
    bool wasChannelRunning, bool willChannelRun) const
{
    if (willChannelRun) {
        if (m_isStarted && m_runningChannelsNum == 0 && willChannelRun)
            g_temp.currentAnimations.insert(this);
        if (!wasChannelRunning && willChannelRun)
            m_runningChannelsNum++;
    } else {
        if (wasChannelRunning)
            m_runningChannelsNum--;
        if (!m_runningChannelsNum)
            g_temp.currentAnimations.erase(this);
    }
}

bool AnimationManager::isRunning(const AnimationManager::Channel& channel) const
{
    return !channel.animations.empty() && !channel.isPaused && channel.speed > 0;
}

} }
