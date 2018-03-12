/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/JsonFormat.h>
#include <fstream>

namespace Json {
class Value;
}

namespace gamebase { namespace impl {

class GAMEBASE_API JsonSerializer : public ISerializer {
public:
    JsonSerializer();
    ~JsonSerializer();

    virtual void writeFloat(const std::string& name, float f) override;

    virtual void writeDouble(const std::string& name, double d) override;

    virtual void writeInt(const std::string& name, int i) override;
    
    virtual void writeUInt(const std::string& name, unsigned int i) override;

    virtual void writeInt64(const std::string& name, int64_t i) override;

    virtual void writeUInt64(const std::string& name, uint64_t i) override;

    virtual void writeBool(const std::string& name, bool b) override;

    virtual void writeString(const std::string& name, const std::string& value) override;

    virtual void startObject(const std::string& name) override;

    virtual void finishObject() override;

    virtual void startArray(const std::string& name, SerializationTag::Type) override;

    virtual void finishArray() override;

    std::string toString(JsonFormat::Enum format);

private:
    Json::Value* last();

    std::unique_ptr<Json::Value> m_root;
    std::vector<Json::Value*> m_stack;
    bool m_isArrayMode;
};

template <typename T>
std::string serializeToJson(
    const T& obj,
    SerializationMode mode = SerializationMode::Default)
{
    JsonSerializer baseSerializer;
    Serializer serializer(&baseSerializer, mode);
    serializer << "" << obj;
    auto format = mode == SerializationMode::Compressed
        ? JsonFormat::Fast : JsonFormat::Styled;
    return baseSerializer.toString(format);
}

template <typename T>
void serializeToJsonFile(
    const T& obj, SerializationMode mode, const std::string& fname)
{
    std::ofstream file(fname);
    file << serializeToJson(obj, mode);
}

} }
