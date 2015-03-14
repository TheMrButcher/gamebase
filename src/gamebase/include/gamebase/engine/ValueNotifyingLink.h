#pragma once

#include <gamebase/engine/Value.h>
#include <functional>

namespace gamebase {

template <typename T>
class ValueNotifyingLink : public Value<T> {
public:
    ValueNotifyingLink(T* link, const std::function<void()>& notifier)
        : m_link(link)
        , m_notifier(notifier)
    {}

    virtual T get() const
    {
        return *m_link;
    }

    virtual void set(const T& value)
    {
        *m_link = value;
        m_notifier();
    }

private:
    T* m_link;
    std::function<void()> m_notifier;
};

}
