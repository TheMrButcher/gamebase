/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/tools/Delayed.h>
#include "src/impl/global/GlobalTemporary.h"

#include <iostream>

namespace gamebase {

namespace {
const int INVALID_ID = -1;
}

void Handle::cancel()
{
    if (isValid()) {
        std::cout << "Canceling " << *m_id << std::endl;
        impl::g_temp.delayedTasks[*m_id] = nullptr;
        m_id.reset();
    }
}

bool Handle::isValid()
{
    return m_id != nullptr && *m_id >= 0;
}

Handle::operator bool()
{
    return isValid();
}

Handle::Handle() {}

Handle::Handle(const std::function<void()>& func)
{
    m_id = std::make_unique<int>(impl::g_temp.delayedTasks.size());
    impl::g_temp.delayedTasks.push_back([func, id = m_id.get()]()
    {
        int idValue = *id;
        std::cout << "Started " << idValue << std::endl;
        *id = INVALID_ID;
        func();
        std::cout << "Finished " << idValue << std::endl;
    });
    std::cout << "Created " << *m_id << std::endl;
}

Handle::~Handle()
{
    cancel();
}

Handle::Handle(Handle&& handle)
    : m_id(std::move(handle.m_id))
{}

Handle& Handle::operator=(Handle&& handle)
{
    cancel();
    m_id = std::move(handle.m_id);
    return *this;
}

void execDelayed(const std::function<void()>& func)
{
    impl::g_temp.delayedTasks.push_back(func);
}

}
