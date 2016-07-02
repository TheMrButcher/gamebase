#pragma once

#include <gamebase/GameBaseAPI.h>
#include <stdint.h>

namespace gamebase {

typedef uint64_t Time;

class GAMEBASE_API PreciseTimer {
public:
    PreciseTimer();

    void start();

    double time() const;

private:
    Time m_startTicks;
};

// Time from application's start in milliseconds
GAMEBASE_API Time currentTime();

}
