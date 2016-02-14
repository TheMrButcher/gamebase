#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/TimeState.h>
#include <gamebase/engine/IAnimation.h>
#include <map>
#include <deque>

namespace gamebase {

class GAMEBASE_API AnimationManager {
public:
    AnimationManager(TimeState::Type type = TimeState::Real)
        : m_isStarted(false)
        , m_type(type)
    {}
    ~AnimationManager();
    
    void start();
    void addAnimation(const std::shared_ptr<IAnimation>& animation, int channelID = 0);
    void step() const;
    void resetChannel(int channelID);
    void reset();

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
    bool m_isStarted;
    mutable std::map<int, Channel> m_channels;
    TimeState::Type m_type;
};

}
