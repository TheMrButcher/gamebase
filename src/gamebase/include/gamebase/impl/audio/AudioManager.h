/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/audio/IAudio.h>
#include <gamebase/impl/audio/AudioChannel.h>
#include <boost/optional.hpp>
#include <deque>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>

namespace gamebase { namespace impl {

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    std::shared_ptr<IAudio> addAudio(const std::string& filePath, int channelID);
    std::shared_ptr<IAudio> loopAudio(const std::string& filePath, int channelID);

    void step();
    void resetChannel(int channelID);
    void reset();

    void setSpeed(float speed, int channelID);
    void setSpeed(float speed);
    float speed(int channelID) const;
    float speed() const { return m_speed; }

    void setVolume(float volume, int channelID);
    void setVolume(float volume);
    float volume(int channelID) const;
    float volume() const { return m_volume; }

    void pause(int channelID);
    void pause();
    void resume(int channelID);
    void resume();
    bool isPaused(int channelID) const;
    bool isPaused() const { return m_isPaused; }

    bool isEmpty(int channelID) const;
    bool isRunning(int channelID) const;

private:
    std::map<int, AudioChannel> m_channels;

    float m_speed;
    float m_volume;
    bool m_isPaused;

    enum class Type {
        Sound,
        Music
    };
    std::unordered_map<std::string, Type> m_pathToType;
};

} }
