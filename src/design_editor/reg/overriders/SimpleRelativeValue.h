/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <reg/ClassRegistrar.h>
#include <gamebase/impl/engine/RelativeValue.h>

namespace gamebase { namespace editor {

struct SimpleRelType {
    enum Enum {
        Pixels,
        Percents
    };
};

class SimpleRelativeValue : public impl::ISerializable {
public:
    SimpleRelativeValue()
        : m_type(SimpleRelType::Pixels)
        , m_value(0)
    {}

    SimpleRelativeValue(SimpleRelType::Enum type, float value)
    {
        set(type, value);
    }

    SimpleRelativeValue(impl::RelType::Enum type, float value)
    {
        set(type, value);
    }

    SimpleRelativeValue(const impl::RelativeValue& value)
    {
        set(value.type(), value.value());
    }
    
    void set(impl::RelType::Enum type, float value);
    void set(SimpleRelType::Enum type, float value);

    impl::RelativeValue toRelativeValue() const;

    SimpleRelType::Enum type() const { return m_type; }
    float value() const;

    virtual void serialize(impl::Serializer&) const override;

private:
    SimpleRelType::Enum m_type;
    int m_value;
};

} }
