/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/tools/PreciseTimer.h>

namespace gamebase {

PreciseTimer::PreciseTimer()
{
    start();
}

void PreciseTimer::start()
{
    m_startTicks = currentTime();
}

double PreciseTimer::time() const
{
    return (currentTime() - m_startTicks) / 1000.0;
}

uint64_t currentTime()
{
    return static_cast<Time>(GetTickCount64());
}

}
