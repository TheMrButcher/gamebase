/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/app/TimeState.h>
#include <functional>

namespace gamebase { namespace impl {

class TimerSharedState {
public:
    TimerSharedState(TimeState::Type type = TimeState::Real)
        : m_type(type)
        , m_inQueue(false)
    {
        start();
    }

    void start()
    {
        m_startTime = TimeState::time(m_type).value;
        m_offset = 0;
        m_paused = false;
    }

    void stop()
    {
        start();
        pause();
    }

    bool isPaused() const
    {
        return m_paused;
    }

    void pause()
    {
        m_offset = static_cast<int64_t>(time());
        m_paused = true;
    }

    void resume()
    {
        m_startTime = TimeState::time(m_type).value;
        m_paused = false;
    }

    Time time() const
    {
        return m_paused
            ? static_cast<uint64_t>(m_offset)
            : static_cast<uint64_t>(TimeState::time(m_type).value - m_startTime + m_offset);
    }

    bool isPeriodical() const
    {
        return m_period != 0 && m_callback;
    }

    bool shift()
    {
        if (m_period == 0)
            return false;
        if (time() > m_period) {
            m_offset -= m_period;
            return true;
        }
        return false;
    }

    bool shiftPeriodInQueue()
    {
        if (!isPeriodical())
            return false;
        if (time() > m_period) {
            m_offset -= m_period;
            if (m_callback)
                m_callback();
            return true;
        }
        return false;
    }

    void setPeriod(Time period)
    {
        m_period = period;
    }

    void setCallback(const std::function<void()>& callback)
    {
        m_callback = callback;
    }

    void setType(TimeState::Type type)
    {
        m_type = type;
        start();
    }
    TimeState::Type type() const { return m_type; }

    void setInQueue(bool value) { m_inQueue = value; }
    bool isInQueue() const { return m_inQueue; }

private:
    TimeState::Type m_type;
    Time m_startTime;
    int64_t m_offset;
    Time m_period;
    bool m_paused;
    std::function<void()> m_callback;
    bool m_inQueue;
};

} }
