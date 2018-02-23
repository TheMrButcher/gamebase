/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/app/TimeState.h>
#include <functional>
#include <memory>

namespace gamebase { namespace impl {

class TimerSharedState;

class GAMEBASE_API Timer {
public:
    Timer(TimeState::Type type = TimeState::Real);

    void start();
    void stop();
    bool isPaused() const;
    void pause();
    void resume();
    Time time() const;
    bool shift();
    void setPeriod(Time period);
    void setCallback(const std::function<void()>& callback);
    void setType(TimeState::Type type);
    TimeState::Type type() const;

private:
    void startPeriodicalUpdates();

    std::shared_ptr<TimerSharedState> m_state;
};

} }
