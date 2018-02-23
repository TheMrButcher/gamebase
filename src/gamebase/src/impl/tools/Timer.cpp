/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/tools/Timer.h>
#include "TimerSharedState.h"
#include "src/impl/global/GlobalTemporary.h"

namespace gamebase { namespace impl {

Timer::Timer(TimeState::Type type)
    : m_state(std::make_shared<TimerSharedState>(type))
{}

void Timer::start()
{
    m_state->start();
}

void Timer::stop()
{
    m_state->stop();
}

bool Timer::isPaused() const
{
    return m_state->isPaused();
}

void Timer::pause()
{
    m_state->pause();
}

void Timer::resume()
{
    m_state->resume();
}

Time Timer::time() const
{
    return m_state->time();
}

bool Timer::shift()
{
    return m_state->shift();
}

void Timer::setPeriod(Time period)
{
    m_state->setPeriod(period);
    startPeriodicalUpdates();
}

void Timer::setCallback(const std::function<void()>& callback)
{
    m_state->setCallback(callback);
    startPeriodicalUpdates();
}

void Timer::setType(TimeState::Type type)
{
    m_state->setType(type);
}

TimeState::Type Timer::type() const
{
    return m_state->type();
}

void Timer::startPeriodicalUpdates()
{
    if (m_state->isPeriodical() && !m_state->isInQueue()) {
        g_temp.timers.push_back(m_state);
        m_state->setInQueue(true);
    }
}

} }
