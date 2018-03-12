/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/tools/Timer.h>

namespace gamebase {

class GAMEBASE_API Task {
public:
    void cancel();
    bool isAlive() const;
    operator bool() const;

public:
    Task() = default;
    Task(float time, const std::function<void()>& task);

private:
    impl::Timer m_timer;
};

#define callOnce(TIME, FUNCTION_NAME, ...) \
    Task(TIME, [=]() { FUNCTION_NAME(##__VA_ARGS__); })

}
