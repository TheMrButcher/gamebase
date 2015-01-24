#pragma once

#include <gamebase/engine/IValue.h>

namespace gamebase {

class FloatValue : public IValue {
public:
    virtual float get() const = 0;

    virtual void set(float value) = 0; 
};

}
