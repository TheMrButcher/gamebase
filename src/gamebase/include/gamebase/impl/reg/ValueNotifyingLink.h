/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/reg/Value.h>
#include <functional>

namespace gamebase { namespace impl {

template <typename T>
class ValueNotifyingLink : public Value<T> {
public:
    ValueNotifyingLink(T* link, const std::function<void()>& notifier)
        : m_link(link)
        , m_notifier(notifier)
    {}

    virtual T get() const
    {
        return *m_link;
    }

    virtual void set(const T& value)
    {
        *m_link = value;
        m_notifier();
    }

private:
    T* m_link;
    std::function<void()> m_notifier;
};

} }
