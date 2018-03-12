/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameview/IDatabase.h>
#include <unordered_map>

namespace gamebase { namespace impl {

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

} }
