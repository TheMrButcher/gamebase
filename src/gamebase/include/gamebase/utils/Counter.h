#pragma once

#include <gamebase/utils/Timer.h>
#include <string>

namespace gamebase {

class Counter {
public:
    Counter(const std::string& message = "Counter", double period = 1.0);

    void start();

    void touch();

private:
    std::string m_message;
    double m_period;
    Timer m_timer;
    size_t m_counts;
};

}
