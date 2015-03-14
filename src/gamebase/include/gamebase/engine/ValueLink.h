#pragma once

#include <gamebase/engine/Value.h>

namespace gamebase {

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

}
