#pragma once

#include <gamebase/engine/IDatabase.h>
#include <unordered_map>

namespace gamebase {

template <typename DataType>
class Database : public IDatabase {
public:
    Database() {}

    virtual DataType& get(int id) { return m_db[id]; }
    virtual const DataType& get(int id) const { return m_db.at(id); }
    virtual void remove(int id) override { m_db.erase(id); }
    virtual void clear() { m_db.clear(); }

private:
    std::unordered_map<int, DataType> m_db;
};

}
