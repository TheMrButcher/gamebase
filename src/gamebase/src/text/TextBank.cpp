#include <stdafx.h>
#include <gamebase/text/TextBank.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

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

}
