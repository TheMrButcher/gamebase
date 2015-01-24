#pragma once

#include <gamebase/engine/FloatValue.h>

namespace gamebase {

class FloatPointingValue : public FloatValue {
public:
    FloatPointingValue(float* value)
        : m_value(value)
    {}

    virtual float get() const override { return *m_value; }

    virtual void set(float value) override { *m_value = value; }

private:
    float* m_value;
};

}
