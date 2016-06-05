#pragma once

#include <gamebase/impl/anim/IAnimation.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

template <typename T>
class InstantChange : public IAnimation, public ISerializable {
public:
    InstantChange(const std::string& propName, const T& newValue)
        : m_propName(propName)
        , m_newValue(newValue)
    {}

    virtual void load(const PropertiesRegister& props) override
    {
        m_property = props.getProperty<T>(m_propName);
    }

    virtual void start() override
    {
        m_done = false;
    }

    virtual Time step(Time t) override
    {
        if (m_done)
            return t;
        m_done = true;
        m_property->set(m_newValue);
        return t;
    }

    virtual bool isFinished() const override
    {
        return m_done;
    }

    virtual void serialize(Serializer& s) const override
    {
        s << "propertyName" << m_propName << "newValue" << m_newValue;
    }

private:
    std::string m_propName;
    std::shared_ptr<Value<T>> m_property;
    T m_newValue;
    bool m_done;
};

} }
