#pragma once

#include <gamebase/impl/reg/Value.h>
#include <functional>

namespace gamebase { namespace impl {

template <typename T, typename SetterType>
class ValueLinkWithSetter : public Value<T> {
public:
    ValueLinkWithSetter(T* link, const SetterType& setter)
        : m_link(link)
        , m_setter(setter)
    {}

    virtual T get() const
    {
        return *m_link;
    }

    virtual void set(const T& value)
    {
        m_setter(value);
    }

private:
    T* m_link;
    SetterType m_setter;
};

} }
