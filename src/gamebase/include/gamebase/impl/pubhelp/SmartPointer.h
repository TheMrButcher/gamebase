#pragma once

#include <gamebase/tools/Exception.h>
#include <memory>

namespace gamebase { namespace impl {

template <typename T>
class SmartPointer {
public:
    SmartPointer()
        : m_ptr(nullptr)
    {}

    SmartPointer(std::nullptr_t)
        : m_ptr(nullptr)
    {}

    SmartPointer(const SmartPointer<T>& other)
        : m_ptr(other.m_ptr)
        , m_sharedPtr(other.m_sharedPtr)
    {}

    template <typename U>
    SmartPointer(const SmartPointer<U>& other)
    {
        *this = other;
    }

    SmartPointer& operator=(const SmartPointer<T>& other)
    {
        m_ptr = other.m_ptr;
        m_sharedPtr = other.m_sharedPtr;
        return *this;
    }

    template <typename U>
    SmartPointer& operator=(const SmartPointer<U>& other)
    {
        if (other.m_sharedPtr) {
            m_sharedPtr = std::dynamic_pointer_cast<T>(other.m_sharedPtr);
            m_ptr = m_sharedPtr.get();
        } else if (other.m_ptr) {
            m_ptr = dynamic_cast<T*>(other.m_ptr);
        } else {
            m_ptr = nullptr;
        }
        return *this;
    }

    /**
     * Create instance that doesn't share ownage.
     */
    explicit SmartPointer(T* ptr)
        : m_ptr(ptr)
    {}

    /**
     * Create instance that shares ownage.
     */
    explicit SmartPointer(const std::shared_ptr<T>& ptr)
        : m_ptr(ptr.get())
        , m_sharedPtr(ptr)
    {}

    bool isEmpty() const
    {
        return !m_ptr;
    }

    bool isRaw() const
    {
        return !m_sharedPtr;
    }

    bool isShared() const
    {
        return m_sharedPtr;
    }

    T* get() const
    {
        if (!m_ptr)
            THROW_EX() << "Can't get pointer to " << typeid(T).name() << ". Smart pointer is empty";
        return m_ptr;
    }

    const std::shared_ptr<T>& getShared() const
    {
        if (!m_sharedPtr)
            THROW_EX() << "Can't get shared pointer to " << typeid(T).name() << ". Smart pointer doesn't share ownage";
        return m_sharedPtr;
    }

    T* operator->() const
    {
        return get();
    }

    T& operator*() const
    {
        return *get();
    }

    operator bool() const
    {
        return m_ptr != nullptr;
    }

private:
    template <typename U> friend class SmartPointer;

    T* m_ptr;
    std::shared_ptr<T> m_sharedPtr;
};

} }
