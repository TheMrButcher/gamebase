#pragma once

#include <gamebase/serial/ISerializable.h>
#include <string>

namespace gamebase {

class ISerializer {
public:
    virtual ~ISerializer() {}

    virtual void writeFloat(const std::string& name, float f) = 0;

    virtual void writeDouble(const std::string& name, double d) = 0;

    virtual void writeInt(const std::string& name, int i) = 0;

    virtual void writeBool(const std::string& name, bool b) = 0;

    virtual void writeString(const std::string& name, const std::string& value) = 0;

    virtual void writeObject(const std::string& name, ISerializable* obj) = 0;

    virtual void startObject(const std::string& name) = 0;

    virtual void finishObject() = 0;

    virtual void startArray(const std::string& name) = 0;

    virtual void finishArray() = 0;
};

}
