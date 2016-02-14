#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IAnimation.h>
#include <gamebase/engine/TimeState.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API AnimationPause : public IAnimation, public ISerializable {
public:
    AnimationPause(Time time)
        : m_period(time)
    {}

    virtual void load(const PropertiesRegister& props) override {}
    virtual void start() override { m_cur = 0; }
    virtual Time step(Time time) override
    {
        m_cur += time;
        return m_cur < m_period ? 0 : m_cur - m_period;
    }
    virtual bool isFinished() const override { return m_cur >= m_period; }
    virtual void serialize(Serializer& serializer) const override;

private:
    Time m_period;
    Time m_cur;
};

}
