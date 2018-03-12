/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <functional>
#include <memory>

namespace gamebase { namespace impl {

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

} }
