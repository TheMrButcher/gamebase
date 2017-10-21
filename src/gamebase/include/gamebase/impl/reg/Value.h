/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/reg/IValue.h>

namespace gamebase { namespace impl {

template <typename T>
class Value : public IValue {
public:
    virtual T get() const = 0;

    virtual void set(const T& value) = 0;
};

} }
