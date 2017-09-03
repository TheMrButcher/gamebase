/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <functional>
#include <memory>

namespace gamebase {

class GAMEBASE_API Handle {
public:
    void cancel();
    bool isValid();
    operator bool();

public:
    Handle();
    Handle(const std::function<void()>& func);
    ~Handle();
    Handle(Handle&&);
    Handle& operator=(Handle&&);
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

private:
    std::unique_ptr<int> m_id;
};

GAMEBASE_API void execDelayed(const std::function<void()>& func);

}
