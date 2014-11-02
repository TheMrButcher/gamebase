#include <stdafx.h>
#include <gamebase/utils/Counter.h>
#include <iostream>

namespace gamebase {

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
        std::cout << m_message << ": " << m_counts << std::endl;
        start();
    }
}

}
