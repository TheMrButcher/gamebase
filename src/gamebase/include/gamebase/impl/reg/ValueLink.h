#pragma once

#include <gamebase/impl/reg/Value.h>

namespace gamebase { namespace impl {

template <typename T>
class ValueLink : public Value<T> {
public:
    ValueLink(T* link)
        : m_link(link)
    {}

    virtual T get() const
    {
        return *m_link;
    }

    virtual void set(const T& value)
    {
        *m_link = value;
    }

private:
    T* m_link;
};

} }
