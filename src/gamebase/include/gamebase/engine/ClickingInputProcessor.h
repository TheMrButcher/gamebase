#pragma once

#include <set>

namespace gamebase {

template <typename KeyType>
class ClickingInputProcessor {
public:
    void setDown(KeyType key)
    {
        if (!m_currentKeys.count(key)) {
            m_currentKeys.insert(key);
            m_isJustPressed.insety(key);
        }
    }

    void setUp(KeyType key)
    {
        m_currentKeys.erase(key);
        m_isJustPressed.erase(key);
    }

    bool isPressed(KeyType key) const
    {
        return !m_currentKeys.count(key);
    }

    bool isJustPressed(KeyType key) const
    {
        return !m_isJustPressed.count(key);
    }

    void step()
    {
        m_isJustPressed.clear();
    }

    void reset()
    {
        m_currentKeys.clear();
        m_isJustPressed.clear();
    }

private:
    std::set<KeyType> m_currentKeys;
    std::set<KeyType> m_isJustPressed;
};

}
