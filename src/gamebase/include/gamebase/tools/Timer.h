/**
 * Copyright (c) 2017 Slavnejshev Filipp
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
	bool isPaused() const;
	void pause();
	void resume();
	float time() const;
	float seconds() const;
	Time milliseconds() const;
    void repeat(float period);
    bool check();
	bool shift();
	void setPeriod(float period);
	void setCallback(const std::function<void()>& callback);
    
private:
    impl::Timer m_impl;
};

/////////////// IMPLEMENTATION ///////////////////

inline void Timer::start() { m_impl.start(); }
inline void Timer::stop() { m_impl.stop(); }
inline bool Timer::isRunning() const { return !isPaused(); }
inline bool Timer::isPaused() const { return m_impl.isPaused(); }
inline void Timer::pause() { m_impl.pause(); }
inline void Timer::resume() { m_impl.resume(); }
inline float Timer::time() const { return seconds(); }
inline float Timer::seconds() const { return m_impl.time() / 1000.f; }
inline Time Timer::milliseconds() const { return m_impl.time(); }
inline void Timer::repeat(float period) { setPeriod(period); start(); }
inline bool Timer::check() { return m_impl.shift(); }
inline bool Timer::shift() { return m_impl.shift(); }
inline void Timer::setPeriod(float period) { m_impl.setPeriod(static_cast<Time>(period * 1000 + 0.5f)); }
inline void Timer::setCallback(const std::function<void()>& callback) { m_impl.setCallback(callback); }

}
