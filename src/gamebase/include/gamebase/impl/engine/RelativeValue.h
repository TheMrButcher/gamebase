/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/tools/Exception.h>

namespace gamebase { namespace impl {

struct RelType {
    enum Enum {
        Identic,
        Pixels,
        Ratio,
        ValueMinusPixels,
        ValuePlusPixels
    };
};

class RelativeValue {
public:
    RelativeValue()
        : m_type(RelType::Identic)
        , m_value(0.0f)
    {}

    RelativeValue(RelType::Enum type, float value)
        : m_type(type)
        , m_value(value)
    {}

    float count(float originValue) const
    {
        switch (m_type) {
            case RelType::Identic: return originValue;
            case RelType::Pixels: return m_value;
            case RelType::Ratio: return originValue * m_value;
            case RelType::ValueMinusPixels: return originValue - m_value;
            case RelType::ValuePlusPixels: return originValue + m_value;
            default: THROW_EX() << "Bad RelativeValue::Type value: " << static_cast<int>(m_type);
        }
    }

    RelType::Enum type() const { return m_type; }
    float value() const { return m_value; }
    void setValue(float value) { m_value = value; }

private:
    RelType::Enum m_type;
    float m_value;
};

} }
