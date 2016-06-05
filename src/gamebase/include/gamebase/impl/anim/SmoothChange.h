#pragma once

#include <gamebase/impl/anim/IAnimation.h>
#include <gamebase/impl/anim/ChangeFunc.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/math/Transform2.h>
#include <gamebase/tools/Exception.h>
#include <type_traits>

namespace gamebase { namespace impl {

namespace internal {
struct DistCounter {
    DistCounter(float v): result(std::abs(v)) {}

    DistCounter& operator()(float v)
    {
        auto absV = std::abs(v);
        if (absV > result)
            result = absV;
        return *this;
    }

    float result;
};

float distance(float f1, float f2)
{
    return std::abs(f2 - f1);
}

float distance(double f1, double f2)
{
    return static_cast<float>(std::abs(f2 - f1));
}

float distance(Color c1, Color c2)
{
    return DistCounter(c2.r - c1.r)(c2.g - c1.g)(c2.b - c1.b)(c2.a - c1.a).result;
}

float distance(Vec2 v1, Vec2 v2)
{
    return DistCounter(v2.x - v1.x)(v2.y - v1.y).result;
}

template <class T>
typename std::enable_if<std::is_integral<T>::value, float>::type distance(T t1, T t2)
{
    return std::abs(static_cast<float>(t1) - static_cast<float>(t2));
}
}

template <typename T>
class SmoothChange : public IAnimation, public ISerializable {
public:
    SmoothChange(
        const std::string& propName,
        const T& startValue,
        const T& newValue,
        Time time,
        ChangeFunc::Type type)
        : m_propName(propName)
        , m_period(time)
        , m_startValue(startValue)
        , m_newValue(newValue)
        , m_moveToStart(true)
        , m_funcType(type)
    {}

    void setMoveToStart(bool value) { m_moveToStart = value; }

    virtual void load(const PropertiesRegister& props)
    {
        m_property = props.getProperty<T>(m_propName);
    }

    virtual void start()
    {
        if (!m_property)
            THROW_EX() << "Can't start animation, property " << m_propName << " is not loaded";
        if (m_moveToStart) {
            m_curPeriod = m_period;
            m_curStartValue = m_startValue;
        } else {
            m_curStartValue = m_property->get();
            float dist = internal::distance(m_newValue, m_startValue);
            float curDist = internal::distance(m_newValue, m_curStartValue);
            m_curPeriod = (dist > 0 && curDist > 0)
                ? std::max(Time(1), static_cast<Time>(curDist * m_period / dist + 0.99f))
                : Time(1);
        }
        m_cur = 0;
    }

    virtual Time step(Time t)
    {
        m_cur += t;
        float part = m_curPeriod == 0 ? 1.f : clamp(static_cast<float>(m_cur) / m_curPeriod, 0.0f, 1.0f);
        switch (m_funcType) {
            case ChangeFunc::Linear: m_property->set(lerp(m_curStartValue, m_newValue, part)); break;
        }
        return m_cur >= m_curPeriod ? m_cur - m_curPeriod : 0;
    }

    virtual bool isFinished() const
    {
        return m_cur >= m_curPeriod;
    }

    virtual void serialize(Serializer& s) const override
    {
        s << "propertyName" << m_propName << "startValue" << m_startValue
            << "newValue" << m_newValue << "period" << m_period
            << "changeFunc" << m_funcType << "moveToStart" << m_moveToStart;
    }

private:
    std::string m_propName;
    std::shared_ptr<Value<T>> m_property;
    Time m_cur;
    Time m_period;
    T m_startValue;
    T m_newValue;
    ChangeFunc::Type m_funcType;
    bool m_moveToStart;
    
    Time m_curPeriod;
    T m_curStartValue;
};

} }
