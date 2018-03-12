/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/tools/FileIO.h>

namespace Json {
class Value;
}

namespace gamebase { namespace impl {

class GAMEBASE_API JsonDeserializer : public IDeserializer {
public:
    JsonDeserializer(const std::string& jsonStr);
    ~JsonDeserializer();

    static JsonDeserializer fileDeserializer(const std::string& fileName);

    virtual SerializationVersion version() const override;

    virtual bool hasMember(const std::string& name) override;

    virtual float readFloat(const std::string& name) override;

    virtual double readDouble(const std::string& name) override;

    virtual int readInt(const std::string& name) override;

    virtual unsigned int readUInt(const std::string& name) override;

    virtual int64_t readInt64(const std::string& name) override;

    virtual uint64_t readUInt64(const std::string& name) override;

    virtual bool readBool(const std::string& name) override;

    virtual std::string readString(const std::string& name) override;
    
    virtual void startObject(const std::string& name) override;

    virtual void finishObject() override;

    virtual void startArray(const std::string& name, SerializationTag::Type) override;

    virtual size_t arraySize(const std::string& name) override;

    virtual void finishArray() override;

private:
    JsonDeserializer(const std::shared_ptr<Json::Value>& root);
    Json::Value* last();
    Json::Value* member(const std::string& name, bool(Json::Value::*checker)() const, const char* typeName);

    std::shared_ptr<Json::Value> m_root;
    SerializationVersion m_version;
    std::vector<Json::Value*> m_stack;
    bool m_isArrayMode;
    std::vector<size_t> m_arrayIndices;
};

template <typename T>
void deserializeFromJson(const std::string& jsonStr, T& obj)
{
    JsonDeserializer baseDeserializer(jsonStr);
    Deserializer deserializer(&baseDeserializer);
    deserializer >> "root" >> obj;
    g_registryBuilder.registerObject(obj);
}

template <typename T>
void deserializeFromJsonFile(const std::string& fname, T& obj)
{
    deserializeFromJson(loadTextFile(fname), obj);
}

GAMEBASE_API std::string pathToDesign(const std::string& designName);

template <typename T>
std::shared_ptr<T> deserialize(const std::string& fname)
{
    std::shared_ptr<T> obj;
    auto baseDeserializer = JsonDeserializer::fileDeserializer(pathToDesign(fname));
    Deserializer deserializer(&baseDeserializer);
    deserializer >> "root" >> obj;
    g_registryBuilder.registerObject(obj);
    return obj;
}

} }
