#include <stdafx.h>
#include <gamebase/utils/Timer.h>

namespace gamebase {

Timer::Timer()
{
    start();
}

void Timer::start()
{
    m_startTicks = GetTickCount64();
}

double Timer::time() const
{
    return (GetTickCount64() - m_startTicks) / 1000.0;
}

}
