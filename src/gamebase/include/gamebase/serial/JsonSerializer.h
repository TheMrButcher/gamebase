#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/serial/ISerializer.h>
#include <fstream>

namespace Json {
class Value;
}

namespace gamebase {

class GAMEBASE_API JsonSerializer : public ISerializer {
public:
    JsonSerializer();
    ~JsonSerializer();

    virtual void writeFloat(const std::string& name, float f) override;

    virtual void writeDouble(const std::string& name, double d) override;

    virtual void writeInt(const std::string& name, int i) override;

    virtual void writeBool(const std::string& name, bool b) override;

    virtual void writeString(const std::string& name, const std::string& value) override;

    virtual void startObject(const std::string& name) override;

    virtual void finishObject() override;

    virtual void startArray(const std::string& name, SerializationTag::Type) override;

    virtual void finishArray() override;

    enum Format {
        Fast,
        Styled
    };
    std::string toString(Format format);

private:
    Json::Value* last();

    std::unique_ptr<Json::Value> m_root;
    std::vector<Json::Value*> m_stack;
    bool m_isArrayMode;
};

template <typename T>
std::string serializeToJson(const T& obj, JsonSerializer::Format format)
{
    JsonSerializer baseSerializer;
    Serializer serializer(&baseSerializer);
    serializer << "" << obj;
    return baseSerializer.toString(format);
}

template <typename T>
void serializeToJsonFile(
    const T& obj, JsonSerializer::Format format, const std::string& fname)
{
    std::ofstream file(fname);
    file << serializeToJson(obj, format);
}
}
