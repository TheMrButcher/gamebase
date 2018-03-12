/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/tools/CallOnce.h>
#include "src/impl/global/GlobalTemporary.h"

namespace gamebase {

namespace {
Time makeTime(float time)
{
    return static_cast<Time>(time * 1000 + 0.5f);
}
}

void Task::cancel()
{
    impl::g_temp.callOnceTimers.erase(m_timer.sharedState());
    m_timer.stop();
}

bool Task::isAlive() const
{
    return !m_timer.isPaused();
}

Task::operator bool() const
{
    return isAlive();
}

Task::Task(float time, const std::function<void()>& task)
{
    m_timer.setPeriod(makeTime(time));
    std::weak_ptr<impl::TimerSharedState> weakState = m_timer.sharedState();
    m_timer.setCallback([=]()
    {
        if (task)
            task();
        if (auto state = weakState.lock()) {
            state->stop();
            impl::g_temp.callOnceTimers.erase(state);
        }
    });
    impl::g_temp.callOnceTimers.insert(m_timer.sharedState());
}

}
