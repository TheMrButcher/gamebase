#pragma once

#include <gamebase/engine/IAnimation.h>
#include <gamebase/engine/Timer.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/math/Transform2.h>

namespace gamebase {

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
std::enable_if<std::is_integral<T>::value, float>::type distance(T t1, T t2)
{
    return std::abs(static_cast<float>(t1) - static_cast<float>(t2));
}
}

template <typename T>
class SmoothChange {
public:
    enum FuncType {
        Linear
    };

    SmoothChange(
        const std::string& propName,
        const T& startValue,
        const T& newValue,
        TypedTime time,
        FuncType type)
        : m_propName(propName)
        , m_timer(time.type)
        , m_period(time.value)
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
        if (m_moveToStart) {
            m_curPeriod = m_period;
            m_curStartValue = m_startValue;
        } else {
            m_curStartValue = m_property->get();
            float dist = internal::distance(m_newValue, m_startValue);
            float curDist = internal::distance(m_newValue, m_curStartValue);
            m_period = dist > 0
                ? static_cast<Time>(curDist * m_period / dist + 0.99f) : Time(1);
        }
        m_timer.start();
    }

    virtual void step()
    {
        float part = clamp(static_cast<float>(m_timer.time()) / m_curPeriod, 0.0f, 1.0f);
        switch (m_funcType) {
            case Linear: m_property->set(lerp(m_curStartValue, m_newValue, part)); break;
        }
    }

    virtual bool isFinished() const
    {
        return m_timer.time() >= m_curPeriod;
    }

private:
    std::string m_propName;
    std::shared_ptr<Value<T>> m_property;
    Timer m_timer;
    Time m_period;
    T m_startValue;
    T m_newValue;
    FuncType m_funcType;
    bool m_moveToStart;
    
    Time m_curPeriod;
    T m_curStartValue;
};

}
