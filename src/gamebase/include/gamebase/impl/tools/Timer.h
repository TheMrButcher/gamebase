#pragma once

#include <gamebase/impl/app/TimeState.h>

namespace gamebase { namespace impl {

class Timer {
public:
    Timer(TimeState::Type type = TimeState::Real)
        : m_type(type)
    {
        start();
    }

    void start()
    {
        m_startTime = TimeState::time(m_type).value;
    }

    Time time() const
    {
        return TimeState::time(m_type).value - m_startTime;
    }

    bool isPeriod(Time period)
    {
        if (time() > period) {
            m_startTime += period;
            return true;
        }
        return false;
    }

    void setType(TimeState::Type type) { m_type = type; }
    TimeState::Type type() const { return m_type; }

private:
    TimeState::Type m_type;
    Time m_startTime;
};

} }