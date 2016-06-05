#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/common/Time.h>
#include <stdint.h>

namespace gamebase { namespace impl {

class Application;

struct GAMEBASE_API TimeState {
    enum Type {
        Real,
        Game
    };

    Time value;
    Time delta;

    static const TimeState& realTime() { return realTime_; }
    static const TimeState& gameTime() { return gameTime_; }
    static const Time INIFINITY = static_cast<Time>(-1);

    static const TimeState& time(Type type)
    {
        return type == Real ? realTime_ : gameTime_;
    }

private:
    friend class Application;
    static TimeState realTime_;
    static TimeState gameTime_;
};

struct TypedTime {
    TypedTime(): type(TimeState::Real), value(0) {}
    TypedTime(TimeState::Type type, Time time)
        : type(type), value(time) {}

    TimeState::Type type;
    Time value;
};

inline float timeDelta() { return TimeState::realTime().delta / 1000.0f; }

} }
