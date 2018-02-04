/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/serial/JsonDeserializer.h>
#include "src/impl/global/Config.h"
#include "src/impl/global/GlobalCache.h"
#include <json/reader.h>

namespace gamebase { namespace impl {

namespace {
SerializationVersion extractVersion(const Json::Value& value)
{
    if (value.isObject()) {
        if (value.isMember(VERSION_TAG)) {
            if (value[VERSION_TAG].asString() == toString(SerializationVersion::VER3))
                return SerializationVersion::VER3;
        }
    }
    return SerializationVersion::VER2;
}
}

JsonDeserializer::JsonDeserializer(const std::string& jsonStr)
    : m_root(new Json::Value())
    , m_isArrayMode(false)
{
    Json::Reader reader;
    reader.parse(jsonStr, *m_root);
    m_version = extractVersion(*m_root);

    //if (m_version != SerializationVersion::VER3)
    //    std::cerr << "Warning! Legacy version of design: " << jsonStr.substr(0, 100) << std::endl;
}

JsonDeserializer::~JsonDeserializer() {}

JsonDeserializer JsonDeserializer::fileDeserializer(const std::string& fileName)
{
    auto& jsonValue = g_cache.designCache[fileName];
    if (!jsonValue) {
        JsonDeserializer result(loadTextFile(fileName));
        jsonValue = result.m_root;
        return std::move(result);
    }
    return JsonDeserializer(jsonValue);
}

SerializationVersion JsonDeserializer::version() const
{
    return m_version;
}

bool JsonDeserializer::hasMember(const std::string& name)
{
    auto* lastVal = last();
    if (m_isArrayMode)
        return false;
    return lastVal->isMember(name);
}

float JsonDeserializer::readFloat(const std::string& name)
{
    auto* value = member(name, &Json::Value::isDouble, "float");
    return value->asFloat();
}

double JsonDeserializer::readDouble(const std::string& name)
{
    auto* value = member(name, &Json::Value::isDouble, "double");
    return value->asDouble();
}

int JsonDeserializer::readInt(const std::string& name)
{
    auto* value = member(name, &Json::Value::isInt, "int");
    return value->asInt();
}

unsigned int JsonDeserializer::readUInt(const std::string& name)
{
    auto* value = member(name, &Json::Value::isUInt, "unsigned int");
    return value->asUInt();
}

int64_t JsonDeserializer::readInt64(const std::string& name)
{
    auto* value = member(name, &Json::Value::isInt64, "int64_t");
    return value->asInt64();
}

uint64_t JsonDeserializer::readUInt64(const std::string& name)
{
    auto* value = member(name, &Json::Value::isUInt64, "uint64_t");
    return value->asUInt64();
}

bool JsonDeserializer::readBool(const std::string& name)
{
    auto* value = member(name, &Json::Value::isBool, "bool");
    return value->asBool();
}

std::string JsonDeserializer::readString(const std::string& name)
{
    auto* value = member(name, &Json::Value::isString, "string");
    return value->asString();
}
    
void JsonDeserializer::startObject(const std::string& name)
{
    if (m_stack.empty()) {
        m_stack.push_back(m_root.get());
        if (!m_root->isObject())
            THROW_EX() << "Root is not object";
    } else {
        m_stack.push_back(member(name, &Json::Value::isObject, "object"));
    }
    m_isArrayMode = false;
}

void JsonDeserializer::finishObject()
{
    m_stack.pop_back();
    if (!m_stack.empty())
        m_isArrayMode = last()->isArray();
}

void JsonDeserializer::startArray(const std::string& name, SerializationTag::Type)
{
    if (m_stack.empty()) {
        if (!m_root->isArray())
            THROW_EX() << "Root is not array";
        m_stack.push_back(m_root.get());
    } else {
        m_stack.push_back(member(name, &Json::Value::isArray, "array"));
    }
    m_arrayIndices.push_back(0);
    m_isArrayMode = true;
}

size_t JsonDeserializer::arraySize(const std::string& name)
{
    if (m_stack.empty()) {
        if (!m_root->isArray())
            THROW_EX() << "Root is not array";
        return m_root->size();
    } else {
        return member(name, &Json::Value::isArray, "array")->size();
    }
}

void JsonDeserializer::finishArray()
{
    finishObject();
    m_arrayIndices.pop_back();
}

JsonDeserializer::JsonDeserializer(const std::shared_ptr<Json::Value>& root)
    : m_root(root)
    , m_isArrayMode(false)
{
    m_version = extractVersion(*m_root);
}

Json::Value* JsonDeserializer::last()
{
    if (m_stack.empty())
        THROW_EX() << "No object or array in stack. Deserialize it first";
    return m_stack.back();
}
    
Json::Value* JsonDeserializer::member(
    const std::string& name, bool(Json::Value::*checker)() const, const char* typeName)
{
    auto* lastVal = last();
    if (m_isArrayMode) {
        size_t index = m_arrayIndices.back()++;
        if (index >= lastVal->size())
            THROW_EX() << "Can't get " << index << "element. "
                "Current array contains only " << lastVal->size() << " elements";
        auto* result = &(*lastVal)[index];
        if (!(result->*checker)())
            THROW_EX() << "Value with index " << index << " is not of type " << typeName;
        return result;
    }
    if (!lastVal->isMember(name))
        THROW_EX() << "Current object doesn't contain member with name: " << name;
    auto* result = &(*lastVal)[name];
    if (!(result->*checker)())
        THROW_EX() << "Member with name " << name << " is not of type " << typeName;
    return result;
}

std::string pathToDesign(const std::string& designName)
{
    return config().designPath + designName;
}

} }
