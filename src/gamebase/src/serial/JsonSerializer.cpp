#include <stdafx.h>
#include <gamebase/serial/JsonSerializer.h>
#include <json/writer.h>

namespace gamebase {

namespace {
const std::string ROOT_CHILD = "OBJ";

template <typename T>
Json::Value& serialize(Json::Value* value, const std::string& name, const T& t, bool isArrayMode)
{
    if (isArrayMode)
        return value->append(Json::Value(t));
    return ((*value)[name] = t);
}
}

JsonSerializer::JsonSerializer()
    : m_root(new Json::Value(Json::objectValue))
    , m_stack(1, m_root.get())
    , m_isArrayMode(false)
{}

JsonSerializer::~JsonSerializer() {}

void JsonSerializer::writeFloat(const std::string& name, float f)
{
    writeDouble(name, static_cast<double>(f));
}

void JsonSerializer::writeDouble(const std::string& name, double d)
{
    serialize(last(), name, d, m_isArrayMode);
}

void JsonSerializer::writeInt(const std::string& name, int i)
{
    serialize(last(), name, i, m_isArrayMode);
}

void JsonSerializer::writeBool(const std::string& name, bool b)
{
    serialize(last(), name, b, m_isArrayMode);
}

void JsonSerializer::writeString(const std::string& name, const std::string& value)
{
    serialize(last(), name, value, m_isArrayMode);
}

void JsonSerializer::startObject(const std::string& name)
{
    m_stack.push_back(
        &serialize(last(), m_stack.size() == 1 ? ROOT_CHILD : name,
            Json::Value(Json::objectValue), m_isArrayMode));
    m_isArrayMode = false;
}

void JsonSerializer::finishObject()
{
    m_stack.pop_back();
    m_isArrayMode = last()->isArray();
}

void JsonSerializer::startArray(const std::string& name, SerializationTag::Type)
{
    m_stack.push_back(
        &serialize(last(), m_stack.size() == 1 ? ROOT_CHILD : name,
            Json::Value(Json::arrayValue), m_isArrayMode));
    m_isArrayMode = true;
}

void JsonSerializer::finishArray()
{
    finishObject();
}

std::string JsonSerializer::toString(Format format)
{
    if (m_root->size() != 1)
        THROW_EX() << "Root object is in broken state";
    if (!m_root->isMember(ROOT_CHILD))
        THROW_EX() << "Root object is in broken state, can't find member 'OBJ'";
    if (format == Fast) {
        Json::FastWriter writer;
        return writer.write((*m_root)[ROOT_CHILD]);
    }

    Json::StyledWriter writer;
    return writer.write((*m_root)[ROOT_CHILD]);
}

Json::Value* JsonSerializer::last()
{
    return m_stack.back();
}
}
