#include <stdafx.h>
#include <gamebase/engine/AnimationManager.h>
#include "src/core/GlobalTemporary.h"

namespace gamebase {

AnimationManager::~AnimationManager()
{
    reset();
}

void AnimationManager::start()
{
    m_isStarted = true;
    if (!m_channels.empty())
        g_temp.currentAnimations.insert(this);
}

void AnimationManager::addAnimation(const std::shared_ptr<IAnimation>& animation, int channelID)
{
    if (!animation)
        return;
    if (m_isStarted && m_channels.empty())
        g_temp.currentAnimations.insert(this);
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        it = m_channels.insert(std::make_pair(channelID, Channel())).first;
    it->second.animations.push_back(animation);
}

void AnimationManager::step() const
{
    auto time = TimeState::time(m_type).delta;
    for (auto it = m_channels.begin(); it != m_channels.end();) {
        auto& channel = it->second;
        bool needStart = !channel.isStarted;
        channel.isStarted = true;
        while (!channel.animations.empty()) {
            auto& animation = channel.animations.front();
            if (needStart) {
                animation->start();
                needStart = false;
            }
            animation->step(time);
            if (!animation->isFinished())
                break;
            channel.animations.pop_front();
            needStart = true;
        }
        if (channel.animations.empty())
            it = m_channels.erase(it);
        else
            ++it;
    }
    if (m_channels.empty())
        g_temp.currentAnimations.erase(this);
}

void AnimationManager::resetChannel(int channelID)
{
    m_channels.erase(channelID);
    if (m_channels.empty())
        g_temp.currentAnimations.erase(this);
}

void AnimationManager::reset()
{
    if (!m_channels.empty()) {
        m_channels.clear();
        g_temp.currentAnimations.erase(this);
    }
}

}
