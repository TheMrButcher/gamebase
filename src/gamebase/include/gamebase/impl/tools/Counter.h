/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/tools/PreciseTimer.h>
#include <string>

namespace gamebase { namespace impl {

class GAMEBASE_API Counter {
public:
    Counter(const std::string& message = "Counter", double period = 1.0);

    void start();

    void touch();

private:
    std::string m_message;
    double m_period;
    PreciseTimer m_timer;
    size_t m_counts;
};

} }
