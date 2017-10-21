/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/serial/JsonSerializer.h>
#include <json/writer.h>

namespace gamebase { namespace impl {

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

void JsonSerializer::writeUInt(const std::string& name, unsigned int i)
{
    serialize(last(), name, i, m_isArrayMode);
}

void JsonSerializer::writeInt64(const std::string& name, int64_t i)
{
    serialize(last(), name, i, m_isArrayMode);
}

void JsonSerializer::writeUInt64(const std::string& name, uint64_t i)
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
    Json::Value* newValue = nullptr;
    if (m_stack.size() == 1) {
        newValue = &serialize(last(), ROOT_CHILD, Json::Value(Json::objectValue), m_isArrayMode);
        (*newValue)[VERSION_TAG] = SERIALIZATION_VER3_STR;
    } else {
        newValue = &serialize(last(), name, Json::Value(Json::objectValue), m_isArrayMode);
    }
    m_stack.push_back(newValue);
    m_isArrayMode = false;
}

void JsonSerializer::finishObject()
{
    m_stack.pop_back();
    m_isArrayMode = last()->isArray();
}

void JsonSerializer::startArray(const std::string& name, SerializationTag::Type)
{
    if (m_stack.size() == 1)
        THROW_EX() << "Root array is not supported";
    m_stack.push_back(
        &serialize(last(), m_stack.size() == 1 ? ROOT_CHILD : name,
            Json::Value(Json::arrayValue), m_isArrayMode));
    m_isArrayMode = true;
}

void JsonSerializer::finishArray()
{
    finishObject();
}

std::string JsonSerializer::toString(JsonFormat::Enum format)
{
    if (m_root->size() != 1)
        THROW_EX() << "Root object is in broken state";
    if (!m_root->isMember(ROOT_CHILD))
        THROW_EX() << "Root object is in broken state, can't find member 'OBJ'";
    if (format == JsonFormat::Fast) {
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

} }
