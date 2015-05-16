#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/utils/FileIO.h>

namespace Json {
class Value;
}

namespace gamebase {

class GAMEBASE_API JsonDeserializer : public IDeserializer {
public:
    JsonDeserializer(const std::string& jsonStr);
    ~JsonDeserializer();

    virtual float readFloat(const std::string& name) override;

    virtual double readDouble(const std::string& name) override;

    virtual int readInt(const std::string& name) override;

    virtual bool readBool(const std::string& name) override;

    virtual std::string readString(const std::string& name) override;
    
    virtual void startObject(const std::string& name) override;

    virtual void finishObject() override;

    virtual void startArray(const std::string& name, SerializationTag::Type) override;

    virtual void finishArray() override;

private:
    Json::Value* last();
    Json::Value* member(const std::string& name, bool(Json::Value::*checker)() const, const char* typeName);

    std::unique_ptr<Json::Value> m_root;
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
}

template <typename T>
void deserializeFromJsonFile(const std::string& fname, T& obj)
{
    deserializeFromJson(loadTextFile(fname), obj);
}
}
