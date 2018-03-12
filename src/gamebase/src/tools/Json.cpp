/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/tools/Json.h>
#include <gamebase/tools/FileIO.h>
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>
#include <fstream>

namespace jsonlib = Json;

namespace gamebase {

class JsonImpl {
public:
    JsonImpl() : m_value(&m_root) {}
    JsonImpl(bool value) : m_root(value), m_value(&m_root) {}
    JsonImpl(int value) : m_root(value), m_value(&m_root) {}
    JsonImpl(int64_t value) : m_root(value), m_value(&m_root) {}
    JsonImpl(unsigned int value) : m_root(value), m_value(&m_root) {}
    JsonImpl(uint64_t value) : m_root(value), m_value(&m_root) {}
    JsonImpl(float value) : m_root(value), m_value(&m_root) {}
    JsonImpl(double value) : m_root(value), m_value(&m_root) {}
    JsonImpl(const char* value) : m_root(value), m_value(&m_root) {}
    JsonImpl(const std::string& value) : m_root(value), m_value(&m_root) {}
    JsonImpl(const JsonImpl& other) : m_root(*other.m_value), m_value(&m_root) {}

    JsonImpl(jsonlib::Value* value) : m_value(value) {}
    
    JsonImpl& operator=(const JsonImpl& other) { *m_value = *other.m_value; return *this; }

    bool operator==(const JsonImpl& other) const { return *m_value == *other.m_value; }
    bool operator!=(const JsonImpl& other) const { return !(*this == other); }

    bool toBool() const { return m_value->asBool(); }
    int64_t toInt() const { return m_value->asInt64(); }
    double toFloat() const { return m_value->asDouble(); }
    std::string toString() const { return m_value->asString(); }

    bool isBool() const { return m_value->isBool(); }
    bool isInt() const { return m_value->isInt64(); }
    bool isFloat() const { return m_value->isDouble(); }
    bool isString() const { return m_value->isString(); }
    bool isObject() const { return m_value->isObject(); }
    bool isArray() const { return m_value->isArray(); }

    int size() const { return static_cast<int>(m_value->size()); }
    bool empty() const { return m_value->empty(); }
    void clear() { m_value->clear(); }

    bool has(const std::string& name) const { return m_value->isMember(name); }
    void remove(const std::string& name) { m_value->removeMember(name); }
    std::vector<std::string> members() const { return m_value->getMemberNames(); }
    void add(const JsonImpl& value) { m_value->append(*value.m_value); }

    std::shared_ptr<JsonImpl> operator[](int index)
    {
        return std::make_shared<JsonImpl>(&(*m_value)[static_cast<jsonlib::ArrayIndex>(index)]);
    }
    std::shared_ptr<JsonImpl> operator[](const char* name)
    {
        return std::make_shared<JsonImpl>(&(*m_value)[name]);
    }
    std::shared_ptr<JsonImpl> operator[](const std::string& name)
    {
        return std::make_shared<JsonImpl>(&(*m_value)[name]);
    }

    jsonlib::Value* jsonLibValuePtr() { return m_value; }

private:
    jsonlib::Value m_root;
    jsonlib::Value* m_value;
};

bool Json::toBool() const { return m_impl->toBool(); }
int64_t Json::toInt() const { return m_impl->toInt(); }
double Json::toFloat() const { return m_impl->toFloat(); };
std::string Json::toString() const { return m_impl->toString(); }

bool Json::isBool() const { return m_impl->isBool(); }
bool Json::isInt() const { return m_impl->isInt(); }
bool Json::isFloat() const { return m_impl->isFloat(); }
bool Json::isString() const { return m_impl->isString(); }
bool Json::isObject() const { return m_impl->isObject(); }
bool Json::isArray() const { return m_impl->isArray(); }

int Json::size() const { return m_impl->size(); }
bool Json::empty() const { return m_impl->empty(); }
void Json::clear() { m_impl->clear(); }

bool Json::has(const std::string& name) const { return m_impl->has(name); }
void Json::remove(const std::string& name) { m_impl->remove(name); }
std::vector<std::string> Json::members() const { return m_impl->members(); }
void Json::add(const Json& value) { m_impl->add(*value.m_impl); }
void Json::add(const JsonRef& value) { m_impl->add(*value.m_impl); }

JsonRef Json::get(int index) { return JsonRef((*m_impl)[index]); }
JsonRef Json::get(const std::string& name) { return JsonRef((*m_impl)[name]); }
JsonRef Json::get(const char* name) { return (*m_impl)[name]; }

JsonRef Json::operator[](int index) { return JsonRef((*m_impl)[index]); }
JsonRef Json::operator[](const std::string& name) { return JsonRef((*m_impl)[name]); }
JsonRef Json::operator[](const char* name) { return (*m_impl)[name]; }

Json::Json() : m_impl(std::make_shared<JsonImpl>()) {}
Json::Json(bool value) : m_impl(std::make_shared<JsonImpl>(value)) {}
Json::Json(int value) : m_impl(std::make_shared<JsonImpl>(value)) {}
Json::Json(int64_t value) : m_impl(std::make_shared<JsonImpl>(value)) {}
Json::Json(unsigned int value) : m_impl(std::make_shared<JsonImpl>(value)) {}
Json::Json(uint64_t value) : m_impl(std::make_shared<JsonImpl>(value)) {}
Json::Json(float value) : m_impl(std::make_shared<JsonImpl>(value)) {}
Json::Json(double value) : m_impl(std::make_shared<JsonImpl>(value)) {}
Json::Json(const char* value) : m_impl(std::make_shared<JsonImpl>(value)) {}
Json::Json(const std::string& value) : m_impl(std::make_shared<JsonImpl>(value)) {}
Json::Json(const Json& other) : m_impl(std::make_shared<JsonImpl>(*other.m_impl)) {}
Json::Json(Json&& other) : m_impl(std::move(other.m_impl)) {}
Json::Json(const JsonRef& other) : m_impl(std::make_shared<JsonImpl>(*other.m_impl)) {}
Json::Json(JsonRef&& other) : m_impl(std::make_shared<JsonImpl>(*other.m_impl)) {}

Json& Json::operator=(const Json& other) { *m_impl = *other.m_impl; return *this; }
Json& Json::operator=(Json&& other) { m_impl = std::move(other.m_impl); return *this; }
Json& Json::operator=(const JsonRef& other) { *m_impl = *other.m_impl; return *this; }
Json& Json::operator=(JsonRef&& other) { *m_impl = *other.m_impl; return *this; }
bool Json::operator==(const Json& other) const { return *m_impl == *other.m_impl; }
bool Json::operator!=(const Json& other) const { return !(*this == other); }
bool Json::operator==(const JsonRef& other) const { return *m_impl == *other.m_impl; }
bool Json::operator!=(const JsonRef& other) const { return !(*this == other); }

Json::Json(const std::shared_ptr<JsonImpl>& impl) : m_impl(impl) {}

bool JsonRef::toBool() const { return m_impl->toBool(); }
int64_t JsonRef::toInt() const { return m_impl->toInt(); }
double JsonRef::toFloat() const { return m_impl->toFloat(); };
std::string JsonRef::toString() const { return m_impl->toString(); }

bool JsonRef::isBool() const { return m_impl->isBool(); }
bool JsonRef::isInt() const { return m_impl->isInt(); }
bool JsonRef::isFloat() const { return m_impl->isFloat(); }
bool JsonRef::isString() const { return m_impl->isString(); }
bool JsonRef::isObject() const { return m_impl->isObject(); }
bool JsonRef::isArray() const { return m_impl->isArray(); }

int JsonRef::size() const { return m_impl->size(); }
bool JsonRef::empty() const { return m_impl->empty(); }
void JsonRef::clear() { m_impl->clear(); }

bool JsonRef::has(const std::string& name) const { return m_impl->has(name); }
void JsonRef::remove(const std::string& name) { m_impl->remove(name); }
std::vector<std::string> JsonRef::members() const { return m_impl->members(); }
void JsonRef::add(const Json& value) { m_impl->add(*value.m_impl); }
void JsonRef::add(const JsonRef& value) { m_impl->add(*value.m_impl); }

JsonRef JsonRef::get(int index) { return JsonRef((*m_impl)[index]); }
JsonRef JsonRef::get(const std::string& name) { return JsonRef((*m_impl)[name]); }
JsonRef JsonRef::get(const char* name) { return (*m_impl)[name]; }

JsonRef JsonRef::operator[](int index) { return JsonRef((*m_impl)[index]); }
JsonRef JsonRef::operator[](const std::string& name) { return JsonRef((*m_impl)[name]); }
JsonRef JsonRef::operator[](const char* name) { return (*m_impl)[name]; }

JsonRef::JsonRef() {}
JsonRef::JsonRef(const JsonRef& other) : m_impl(other.m_impl) {}
JsonRef::JsonRef(JsonRef&& other) : m_impl(std::move(other.m_impl)) {}

JsonRef& JsonRef::operator=(const Json& other)
{
    if (!m_impl)
        m_impl = other.m_impl;
    else
        *m_impl = *other.m_impl;
    return *this;
}
JsonRef& JsonRef::operator=(Json&& other)
{
    if (!m_impl)
        m_impl = std::move(other.m_impl);
    else
        *m_impl = *other.m_impl;
    return *this;
}
JsonRef& JsonRef::operator=(const JsonRef& other)
{
    if (!m_impl)
        m_impl = other.m_impl;
    else
        *m_impl = *other.m_impl;
    return *this;
}
JsonRef& JsonRef::operator=(JsonRef&& other)
{
    if (!m_impl)
        m_impl = std::move(other.m_impl);
    else
        *m_impl = *other.m_impl;
    return *this;
}
bool JsonRef::operator==(const Json& other) const { return *m_impl == *other.m_impl; }
bool JsonRef::operator!=(const Json& other) const { return !(*this == other); }
bool JsonRef::operator==(const JsonRef& other) const { return *m_impl == *other.m_impl; }
bool JsonRef::operator!=(const JsonRef& other) const { return !(*this == other); }

JsonRef::JsonRef(const std::shared_ptr<JsonImpl>& impl) : m_impl(impl) {}

namespace impl {
GAMEBASE_API std::string pathToDesign(const std::string& designName);
}

void Json::load(const std::string& fileName)
{
    auto fullPath = impl::pathToDesign(fileName);
    auto text = loadTextFile(fullPath);
    parse(text);
}

void Json::save(const std::string& fileName, JsonFormat format) const
{
    auto fullPath = impl::pathToDesign(fileName);
    auto text = serialize(format);

    std::ofstream file(fullPath);
    file << text;
}

void Json::parse(const std::string & text)
{
    auto jsonImpl = std::make_shared<JsonImpl>();
    jsonlib::Reader reader;
    reader.parse(text, *jsonImpl->jsonLibValuePtr());
    m_impl = jsonImpl;
}

std::string Json::serialize(JsonFormat format) const
{
    std::string text;
    if (format == FastJsonFormat) {
        jsonlib::FastWriter writer;
        return writer.write(*m_impl->jsonLibValuePtr());
    }
    jsonlib::StyledWriter writer;
    return writer.write(*m_impl->jsonLibValuePtr());
}

Json loadJsonFromFile(const std::string& fileName)
{
    Json json;
    json.load(fileName);
    return json;
}

Json loadJsonFromString(const std::string & text)
{
    Json json;
    json.parse(text);
    return json;
}

void saveJsonToFile(const std::string& fileName, const Json& json, JsonFormat format)
{
    json.save(fileName, format);
}

std::string toString(const Json& json, JsonFormat format)
{
    return json.serialize(format);
}

}
