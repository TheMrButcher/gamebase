#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IAnimation.h>
#include <gamebase/engine/Timer.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API AnimationPause : public IAnimation, public ISerializable {
public:
    AnimationPause(TypedTime time)
        : m_timer(time.type)
        , m_period(time.value)
    {}

    virtual void load(const PropertiesRegister& props) override {}
    virtual void start() override { m_timer.start(); }
    virtual void step() override {}
    virtual bool isFinished() const override { return m_timer.time() >= m_period; }
    virtual void serialize(Serializer& serializer) const override;

private:
    Timer m_timer;
    Time m_period;
};

}
