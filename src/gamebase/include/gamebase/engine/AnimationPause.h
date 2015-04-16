#pragma once

#include <gamebase/engine/IAnimation.h>
#include <gamebase/engine/Timer.h>

namespace gamebase {

class AnimationPause : public IAnimation {
public:
    AnimationPause(TypedTime time)
        : m_timer(time.type)
        , m_period(time.value)
    {}

    virtual void load(const PropertiesRegister& props) override {}
    virtual void start() override { m_timer.start(); }
    virtual void step() override {}
    virtual bool isFinished() const override { return m_timer.time() >= m_period; }

private:
    Timer m_timer;
    Time m_period;
};

}
