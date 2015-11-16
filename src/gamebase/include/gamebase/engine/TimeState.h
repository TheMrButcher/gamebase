#pragma once

#include <gamebase/GameBaseAPI.h>
#include <stdint.h>

namespace gamebase {

class Application;
typedef uint64_t Time;

struct GAMEBASE_API TimeState {
    enum Type {
        Real,
        Game
    };

    Time value;
    Time delta;

    static const TimeState& realTime() { return realTime_; }
    static const TimeState& gameTime() { return gameTime_; }

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

}
