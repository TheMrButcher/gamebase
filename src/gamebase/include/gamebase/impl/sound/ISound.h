/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/common/Time.h>

namespace gamebase { namespace impl {

class ISound : public virtual IObject {
public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void resume() = 0;
    virtual void pause() = 0;

    virtual bool isRunning() const = 0;
    virtual bool isPaused() const = 0;
    virtual bool isStopped() const = 0;

    virtual void setTime(Time time) = 0;
    virtual Time time() const = 0;
    
    virtual void setSpeed(float speed) = 0;
    virtual float speed() const = 0;

    virtual void setVolume(float volume) = 0;
    virtual float volume() const = 0;

    virtual void setLoop(bool value) = 0;
    virtual bool isLoop() const = 0;

    virtual void kill() = 0;
};

} }
