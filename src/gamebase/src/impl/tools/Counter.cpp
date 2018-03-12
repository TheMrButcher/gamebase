/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/tools/Counter.h>
#include <iostream>
#include <iomanip>

namespace gamebase { namespace impl {

Counter::Counter(const std::string& message, double period)
    : m_message(message)
    , m_period(period)
{
    start();
}

void Counter::start()
{
    m_timer.start();
    m_counts = 0;
}

void Counter::touch()
{
    ++m_counts;
    if (m_timer.time() >= m_period) {
        std::cout
            << m_message << ": "
            << std::fixed << std::setprecision(3) << (m_counts / m_timer.time())
            << std::endl;
        start();
    }
}

} }
