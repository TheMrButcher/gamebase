/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/tools/Register.h>
#include <memory>
#include <unordered_map>
#include <list>

namespace gamebase { namespace impl {

template <typename K, typename V>
class Cache {
public:
    typedef K Key;
    typedef std::shared_ptr<V> Value;
    typedef std::pair<Key, Value> KeyValuePair;
    typedef std::list<KeyValuePair> DataList;

    Cache(size_t maxSize)
        : m_maxSize(maxSize)
    {}

    void insert(const Key& key, const Value& value)
    {
        shrinkToSize(m_maxSize);
        insertNoCheck(key, value);
    }

    void insertNoCheck(const Key& key, const Value& value)
    {
        m_data.emplace_front(KeyValuePair{ key, value });
        m_keyToData[key] = m_data.begin();
    }

    Value get(const Key& key) const
    {
        auto it = m_keyToData.find(key);
        if (it == m_keyToData.end())
            return m_register.get(key);
        return it->second->second;
    }

    size_t maxSize() const
    {
        return m_maxSize;
    }

    void setMaxSize(size_t size)
    {
        m_maxSize = size;
        shrinkToSize(size);
    }

    bool has(const Key& key) const
    {
        return m_keyToData.count(key) > 0 || m_register.has(key);
    }

    void clear()
    {
        m_data.clear();
        m_keyToData.clear();
        m_register.clear();
    }

    void shrinkToSize(size_t size)
    {
        while (m_data.size() > size) {
            const auto& last = m_data.back();
            if (!last.second.unique())
                m_register.insert(last.first, last.second);
            m_keyToData.erase(last.first);
            m_data.pop_back();
        }
    }

private:
    size_t m_maxSize;
    DataList m_data;
    std::unordered_map<Key, typename DataList::iterator> m_keyToData;
    Register<K, V> m_register;
};

} }
