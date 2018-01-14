/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/audio/IAudio.h>
#include <memory>
#include <deque>

namespace gamebase { namespace impl {

struct AudioChannel {
public:
    AudioChannel(float speed, float volume, bool isPaused);
    void add(const std::shared_ptr<IAudio>& audio);
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
    std::deque<std::shared_ptr<IAudio>> m_audioQueue;
    bool m_isParallel;
    float m_speed;
    float m_volume;
    bool m_isPaused;
};

} }
