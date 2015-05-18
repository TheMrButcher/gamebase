#pragma once

#include <gamebase/engine/IAnimation.h>
#include <gamebase/serial/ISerializable.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

template <typename T>
class InstantChange : public IAnimation, public ISerializable {
public:
    InstantChange(const std::string& propName, const T& newValue)
        : m_propName(propName)
        , m_newValue(newValue)
    {}

    virtual void load(const PropertiesRegister& props)
    {
        m_property = props.getProperty<T>(m_propName);
    }

    virtual void start()
    {
        m_done = false;
    }

    virtual void step()
    {
        m_property->set(m_newValue);
        m_done = true;
    }

    virtual bool isFinished() const
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

}
