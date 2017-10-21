/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/anim/IAnimation.h>
#include <gamebase/impl/anim/ChangeFunc.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

template <typename T>
class IncrementalChange : public IAnimation, public ISerializable {
public:
    IncrementalChange(
        const std::string& propName,
        const T& delta,
        Time time,
        ChangeFunc::Type type)
        : m_propName(propName)
        , m_delta(delta)
        , m_period(time)
        , m_funcType(type)
    {}

    virtual void load(const PropertiesRegister& props)
    {
        m_property = props.getProperty<T>(m_propName);
    }

    virtual void start()
    {
        m_curStartValue = m_property->get();
        m_cur = 0;
    }

    virtual Time step(Time t)
    {
        m_cur += t;
        float part = m_period == 0 ? 1 : (static_cast<float>(m_cur) / m_period);
        if (part > 1)
            part = 1;
        switch (m_funcType) {
            case ChangeFunc::Linear: m_property->set(m_curStartValue + part * m_delta); break;
        }
        return m_cur >= m_period ? m_cur - m_period : 0;
    }

    virtual bool isFinished() const
    {
        return m_cur >= m_period;
    }

    virtual void serialize(Serializer& s) const override
    {
        s << "propertyName" << m_propName << "delta" << m_delta
            << "period" << m_period << "changeFunc" << m_funcType;
    }

private:
    std::string m_propName;
    std::shared_ptr<Value<T>> m_property;
    T m_delta;
    Time m_period;
    ChangeFunc::Type m_funcType;
    
    Time m_cur;
    T m_curStartValue;
};

} }
