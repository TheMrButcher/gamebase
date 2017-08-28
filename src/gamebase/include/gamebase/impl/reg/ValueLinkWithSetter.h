/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/reg/Value.h>
#include <functional>

namespace gamebase { namespace impl {

template <typename T>
class ValueLinkWithSetter : public Value<T> {
public:
    ValueLinkWithSetter(T* link, const std::function<void(const T&)>& setter)
        : m_link(link)
        , m_setter(setter)
    {}

    virtual T get() const
    {
        return *m_link;
    }

    virtual void set(const T& value)
    {
        m_setter(value);
    }

private:
    T* m_link;
    std::function<void(const T&)> m_setter;
};

} }
