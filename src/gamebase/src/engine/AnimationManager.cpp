#include <stdafx.h>
#include <gamebase/engine/AnimationManager.h>

namespace gamebase {

void AnimationManager::addAnimation(const std::shared_ptr<IAnimation>& animation, int channelID)
{
    if (!animation)
        return;
    auto it = m_channels.find(channelID);
    if (it == m_channels.end()) {
        animation->start();
        it = m_channels.insert(std::make_pair(channelID, Channel())).first;
    }
    it->second.push_back(animation);
}

void AnimationManager::step() const
{
    for (auto it = m_channels.begin(); it != m_channels.end();) {
        auto& channel = it->second;
        bool needStart = false;
        while (!channel.empty()) {
            auto& animation = channel.front();
            if (needStart) {
                animation->start();
                needStart = false;
            }
            animation->step();
            if (!animation->isFinished())
                break;
            channel.pop_front();
            needStart = true;
        }
        if (channel.empty())
            it = m_channels.erase(it);
        else
            ++it;
    }
}

void AnimationManager::resetChannel(int channelID)
{
    m_channels.erase(channelID);
}

void AnimationManager::reset()
{
    m_channels.clear();
}

}
