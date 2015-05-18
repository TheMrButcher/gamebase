#pragma once

#include <gamebase/engine/TimeState.h>

namespace gamebase {

class Timer {
public:
    Timer(TimeState::Type type)
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

    TimeState::Type type() const { return m_type; }

private:
    TimeState::Type m_type;
    Time m_startTime;
};

}
