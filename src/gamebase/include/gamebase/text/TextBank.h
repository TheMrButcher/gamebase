#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/serial/ISerializable.h>
#include <unordered_map>
#include <string>

namespace gamebase {

class GAMEBASE_API TextBank : public ISerializable {
public:
    void insert(const std::string& key, const std::string& value)
    {
        m_db[key] = value;
    }

    template <typename MapType>
    void insert(const MapType& newValues)
    {
        m_db.insert(newValues.begin(), newValues.end());
    }

    std::string get(const std::string& key) const
    {
        auto it = m_db.find(key);
        if (it == m_db.end())
            return key;
        return it->second;
    }

    virtual void serialize(Serializer& s) const override;

private:
    std::unordered_map<std::string, std::string> m_db;
};

}