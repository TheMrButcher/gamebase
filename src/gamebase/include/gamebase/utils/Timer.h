#pragma once

#include <gamebase/GameBaseAPI.h>
#include <stdint.h>

namespace gamebase {

class GAMEBASE_API Timer {
public:
    Timer();

    void start();

    double time() const;

private:
    uint64_t m_startTicks;
};

}
