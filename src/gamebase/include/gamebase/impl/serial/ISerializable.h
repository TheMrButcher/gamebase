#pragma once

#include <gamebase/impl/engine/IObject.h>

namespace gamebase { namespace impl {

class Serializer;
class Deserializer;

class ISerializable : public virtual IObject {
public:
    virtual void serialize(Serializer& serializer) const = 0;
};

} }
