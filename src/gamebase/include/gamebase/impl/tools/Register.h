/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <memory>
#include <unordered_map>

namespace gamebase { namespace impl {

template <typename K, typename V>
class Register {
public:
    typedef K Key;
    typedef std::shared_ptr<V> Value;

    void insert(const Key& key, const Value& value)
    {
        m_register[key] = value;
        if (m_insertOpCounter++ >= m_register.size())
            shrink();
    }

    Value get(const Key& key) const
    {
        auto it = m_register.find(key);
        if (it == m_register.end())
            return nullptr;
        return it->second.lock();
    }

    bool has(const Key& key) const
    {
        auto it = m_register.find(key);
        if (it == m_register.end())
            return false;
        return !it->second.expired();
    }

    void clear()
    {
        m_register.clear();
        m_insertOpCounter = 0;
    }

private:
    void shrink()
    {
        for (auto it = m_register.begin(); it != m_register.end();) {
            if (it->second.expired()) {
                it = m_register.erase(it);
            } else {
                ++it;
            }
        }
        m_insertOpCounter = 0;
    }

    std::unordered_map<Key, std::weak_ptr<V>> m_register;
    size_t m_insertOpCounter = 0;
};

} }
