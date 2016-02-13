#include <stdafx.h>
#include <gamebase/engine/AnimationManager.h>

namespace gamebase {

void AnimationManager::addAnimation(const std::shared_ptr<IAnimation>& animation, int channelID)
{
    if (!animation)
        return;
    auto it = m_channels.find(channelID);
    if (it == m_channels.end())
        it = m_channels.insert(std::make_pair(channelID, Channel())).first;
    it->second.animations.push_back(animation);
}

void AnimationManager::step() const
{
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
            animation->step();
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
}

}
