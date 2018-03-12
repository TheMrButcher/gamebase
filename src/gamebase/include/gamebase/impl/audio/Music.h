/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/audio/IAudio.h>
#include <string>
#include <memory>

namespace sf {
class Music;
}

namespace gamebase { namespace impl {

class Music : public IAudio {
public:
    Music(const std::string& filePath);
    ~Music();

    virtual void start() override;
    virtual void stop() override;
    virtual void resume() override;
    virtual void pause() override;

    virtual bool isRunning() const override;
    virtual bool isPaused() const override;
    virtual bool isStopped() const override;

    virtual void setTime(Time time) override;
    virtual Time time() const override;

    virtual void setSpeed(float speed) override;
    virtual float speed() const override;

    virtual void setVolume(float volume) override;
    virtual float volume() const override;

    virtual void setLoop(bool value) override;
    virtual bool isLoop() const override;

    virtual void kill() override;

private:
    void ensureMusicCreated();

    std::string m_filePath;
    Time m_time;
    float m_speed;
    float m_volume;
    bool m_loop;
    std::shared_ptr<sf::Music> m_music;
};

} }
