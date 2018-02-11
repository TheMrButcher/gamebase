/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <string>

namespace gamebase {

class GAMEBASE_API AudioManager {
public:
    void run(const std::string& path, int channel = 0);
    void play(const std::string& path, int channel = 0);
    void loop(const std::string& path, int channel = 0);
    void preload(const std::string& path);

    void reset(int channel);
    void reset();
    bool isRunning(int channel) const;
    bool isEmpty(int channel) const;

    void setSpeed(float speed, int channel);
    void setSpeed(float speed);
    float speed(int channel) const;
    float speed() const;

    void setVolume(float volume, int channel);
    void setVolume(float volume);
    float volume(int channel) const;
    float volume() const;

    void pause(int channel);
    void pause();
    void resume(int channel);
    void resume();
    bool isPaused(int channel) const;
    bool isPaused() const;
};

}
