/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/text/TextBank.h>
#include "../global/GlobalResources.h"
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/JsonDeserializer.h>
#include <sstream>

namespace gamebase { namespace impl {

std::string TextBank::get(const std::string& key, int index) const
{
    std::ostringstream ss;
    ss << key << "[" << index << "]";
    return get(ss.str());
}

void TextBank::add(const TextBank& other)
{
    for (const auto& pair : other.m_db)
        insert(pair.first, pair.second);
}

void TextBank::serialize(Serializer& s) const
{
    std::map<std::string, std::string> sortedDB(m_db.begin(), m_db.end());
    s << "database" << sortedDB;
}

std::unique_ptr<IObject> deserializeTextBank(Deserializer& deserializer)
{
    typedef std::map<std::string, std::string> Database;
    DESERIALIZE(Database, database);
    std::unique_ptr<TextBank> result(new TextBank());
    result->insert(database);
    return std::move(result);
}

REGISTER_CLASS(TextBank);

void loadTextBank(const std::string& path)
{
    auto textBank = deserialize<TextBank>(path);
    globalResources().textBank.add(*textBank);
}

TextBank* textBank()
{
    return &globalResources().textBank;
}

} }
