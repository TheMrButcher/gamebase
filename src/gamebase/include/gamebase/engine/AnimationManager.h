#pragma once

#include <gamebase/engine/IAnimation.h>
#include <map>
#include <deque>

namespace gamebase {

class AnimationManager {
public:
    void addAnimation(const std::shared_ptr<IAnimation>& animation, int channelID = 0);
    void step();
    void resetChannel(int channelID);
    void reset();

private:
    typedef std::deque<std::shared_ptr<IAnimation>> Channel;
    std::map<int, Channel> m_channels;
};

}
