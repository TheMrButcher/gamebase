#pragma once

#include <gamebase/engine/IValue.h>

namespace gamebase {

template <typename T>
class Value : public IValue {
public:
    virtual T get() const = 0;

    virtual void set(const T& value) = 0;
};

}
