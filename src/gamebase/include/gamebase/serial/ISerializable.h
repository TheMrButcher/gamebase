#pragma once

#include <gamebase/engine/IObject.h>

namespace gamebase {

class Serializer;
class Deserializer;

class ISerializable : public virtual IObject {
public:
    virtual void serialize(Serializer& serializer) const = 0;
};

}
