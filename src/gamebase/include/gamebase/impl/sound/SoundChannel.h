/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/sound/ISound.h>
#include <memory>
#include <deque>

namespace gamebase { namespace impl {

struct SoundChannel {
public:
    SoundChannel(float speed, float volume, bool isPaused);
    void addSound(const std::shared_ptr<ISound>& sound);
    void step();
    void reset();
    void setParallel(bool value);
    void setSpeed(float speed);
    float speed() const { return m_speed; }
    void setVolume(float volume);
    float volume() const { return m_volume; }
    void pause();
    void resume();
    bool isPaused() const { return m_isPaused; }
    bool isEmpty() const;
    bool isRunning() const;

private:
    std::deque<std::shared_ptr<ISound>> m_sounds;
    bool m_isParallel;
    float m_speed;
    float m_volume;
    bool m_isPaused;
};

} }
