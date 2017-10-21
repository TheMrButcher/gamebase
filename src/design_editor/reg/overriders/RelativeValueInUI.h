/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <reg/ClassRegistrar.h>
#include <gamebase/impl/engine/RelativeValue.h>

namespace gamebase { namespace editor {

class RelativeValueInUI : public impl::ISerializable {
public:
    RelativeValueInUI()
		: m_type(impl::RelType::Pixels)
        , m_value(0)
    {}

    RelativeValueInUI(impl::RelType::Enum type, double value)
    {
        set(type, value);
    }

    explicit RelativeValueInUI(const impl::RelativeValue& value)
    {
        set(value);
    }
    
	void set(const impl::RelativeValue& value)
	{
		set(value.type(), value.value());
	}
    void set(impl::RelType::Enum type, double value);

    impl::RelativeValue toRelativeValue() const;

    impl::RelType::Enum type() const { return m_type; }
    double value() const;
	float valueFloat() const { return static_cast<float>(value()); }

    virtual void serialize(impl::Serializer&) const override;

private:
    impl::RelType::Enum m_type;
    double m_value;
};

} }
