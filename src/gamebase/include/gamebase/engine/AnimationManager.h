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
        , m_runningChannelsNum(0)
        , m_speed(1.0f)
        , m_isPaused(false)
    {}

    ~AnimationManager();
    
    void start();
    void addAnimation(const std::shared_ptr<IAnimation>& animation, int channelID = 0);
    void step() const;
    void resetChannel(int channelID);
    void reset();

    void setSpeed(int channelID, float speed);
    void setSpeed(float speed);
    float speed(int channelID) const;
    float speed() const { return m_speed; }

    void pause(int channelID) const;
    void pause();
    void resume(int channelID) const;
    void resume();
    bool isPaused(int channelID) const;
    bool isPaused() const { return m_isPaused; }

    bool isEmpty(int channelID) const;
    bool isRunning(int channelID) const;

private:
    void countRunningChannels();

    void channelWasChanged(bool wasChannelRunning, bool willChannelRun) const;

    struct Channel {
        Channel(float speed, bool isPaused)
            : isStarted(false)
            , speed(speed)
            , overTime(0)
            , isPaused(isPaused)
        {}

        void reset()
        {
            animations.clear();
            isStarted = false;
            overTime = 0;
        }

        std::deque<std::shared_ptr<IAnimation>> animations;
        bool isStarted;

        float speed;
        float overTime;
        bool isPaused;
    };

    bool isRunning(const Channel& channel) const;

    bool m_isStarted;
    mutable std::map<int, Channel> m_channels;
    TimeState::Type m_type;
    mutable size_t m_runningChannelsNum;

    float m_speed;
    bool m_isPaused;
};

}
