/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/reg/Value.h>
#include <gamebase/tools/Exception.h>
#include <memory>

namespace gamebase { namespace impl {

template <typename T>
class ValueWeakLink : public Value<T> {
public:
    ValueWeakLink(const std::shared_ptr<Value<T>>& value)
        : m_value(value)
    {}

    virtual T get() const override
    {
        if (std::shared_ptr<Value<T>> value = m_value.lock())
            return value->get();
        THROW_EX() << "Can't get value! Weak link is expired";
    }

    virtual void set(const T& value) override
    {
        if (std::shared_ptr<Value<T>> value = m_value.lock())
            return value->set(value);
        THROW_EX() << "Can't set value! Weak link is expired";
    }

private:
    std::weak_ptr<Value<T>> m_value;
};

} }
