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
