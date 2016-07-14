/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/tools/Timer.h>

namespace gamebase {

class Timer {
public:
    void start();
    void stop();
    bool isRunning() const;

    Time time() const;
    float seconds() const;
    bool isPeriod(Time period);
    
public:
    Timer() : m_isRunning(true), m_lastTime(0) {}
private:
    impl::Timer m_impl;
    bool m_isRunning;
    Time m_lastTime;
};

/////////////// IMPLEMENTATION ///////////////////

inline void Timer::start() { m_impl.start(); m_isRunning = true; }
inline void Timer::stop() { m_isRunning = false; m_lastTime = m_impl.time(); }
inline bool Timer::isRunning() const { return m_isRunning; }
inline Time Timer::time() const { return m_isRunning ? m_impl.time() : m_lastTime; }
inline float Timer::seconds() const { return time() / 1000.0f; }
inline bool Timer::isPeriod(Time period) { return m_isRunning ? m_impl.isPeriod(period) : false; }

}
