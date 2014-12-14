#pragma once

#include <set>
#include <vector>

namespace gamebase {

template <typename KeyType>
class SpecificInputRegister {
public:
    void setDown(KeyType key)
    {
        m_signals.push_back(key);
        if (!isPressed(key)) {
            m_currentKeys.insert(key);
            m_isJustPressed.insert(key);
            m_isJustOutpressed.erase(key);
        }
    }

    void setUp(KeyType key)
    {
        if (isPressed(key)) {
            m_currentKeys.erase(key);
            m_isJustPressed.erase(key);
            m_isJustOutpressed.insert(key);
        }
    }

    bool isPressed(KeyType key) const
    {
        return m_currentKeys.count(key) > 0;
    }

    bool isJustPressed(KeyType key) const
    {
        return m_isJustPressed.count(key) > 0;
    }

    bool isJustOutpressed(KeyType key) const
    {
        return m_isJustOutpressed.count(key) > 0;
    }

    const std::vector<KeyType>& signals() const
    {
        return m_signals;
    }

    void step()
    {
        m_isJustPressed.clear();
        m_isJustOutpressed.clear();
        m_signals.clear();
    }

    void reset()
    {
        m_currentKeys.clear();
        m_isJustPressed.clear();
        m_isJustOutpressed.clear();
    }

private:
    std::set<KeyType> m_currentKeys;
    std::set<KeyType> m_isJustPressed;
    std::set<KeyType> m_isJustOutpressed;
    std::vector<KeyType> m_signals;
};

}
