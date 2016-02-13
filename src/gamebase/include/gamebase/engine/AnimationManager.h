#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IAnimation.h>
#include <map>
#include <deque>

namespace gamebase {

class GAMEBASE_API AnimationManager {
public:
    void addAnimation(const std::shared_ptr<IAnimation>& animation, int channelID = 0);
    void step() const;
    void resetChannel(int channelID)
    {
        m_channels.erase(channelID);
    }

    void reset()
    {
        m_channels.clear();
    }

    bool isRunning(int channelID) const
    {
        return m_channels.find(channelID) != m_channels.end();
    }

private:
    struct Channel {
        Channel() : isStarted(false) {}

        std::deque<std::shared_ptr<IAnimation>> animations;
        bool isStarted;
    };
    mutable std::map<int, Channel> m_channels;
};

}
